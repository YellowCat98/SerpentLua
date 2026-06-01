#include <SerpentLua.hpp>
#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;
using namespace SerpentLua::internal;

ServerManager* ServerManager::get() {
	static ServerManager* instance;
	if (!instance) instance = new (std::nothrow) ServerManager();
	return instance;
}

void ServerManager::setServerUrl(const std::string& url) {
	std::string final = url;
	if (!final.empty() && final.back() == '/') {
		final.pop_back();
	}
	this->url = final;
}

void ServerManager::setSessionToken(const std::string& token) {
	this->sessionToken = token;
}

std::string ServerManager::getEndpoint(const std::string& path) {
	bool leadingSlash = false;
	std::string final = path;
	if (!final.empty() && final.front() != '/') {
		leadingSlash = true;
	}

	return fmt::format("{}{}{}", this->url, leadingSlash ? "/" : "", final);
}

web::WebRequest ServerManager::createReq(bool withAuth) {
	web::WebRequest req = web::WebRequest();

	req.timeout(std::chrono::seconds(500));

	if (withAuth) {
		req.header("Authorization", this->sessionToken);
	}

	return req;
}

web::WebFuture ServerManager::sendReq(const std::string& method, const std::string& path, web::WebRequest req) {
	auto url = this->getEndpoint(path);
	return std::move(req).send(method, url);
}

arc::Future<std::pair<web::WebResponse, std::string>> ServerManager::downloadPlugin(bool script, const DisplayInfo& info) {
	using FuckassPair = std::pair<web::WebResponse, std::string>;
	if (info.scriptExample.empty() && script) co_return FuckassPair();

	auto req = this->createReq(false);

	req.param("script", script);
	req.param("id", info.id);

	auto resp = co_await this->sendReq("GET", "/api/v1/plugin/download", req);
		if (resp.ok()) {
			auto dir = Mod::get()->getConfigDir() / "pending_install" / (script ? "scripts" : "plugins");
			auto createdRes = file::createDirectoryAll(dir);
			if (createdRes.isErr()) {
				co_return FuckassPair{resp, createdRes.unwrapErr()};
			}
			auto final = dir / (script ? info.scriptFilename : info.filename);
			if (std::filesystem::exists(final)) {
				std::filesystem::remove_all(final);
			}
			auto res = resp.into(final);
			if (res.isErr()) {
				co_return FuckassPair{resp, res.unwrapErr()};
			}
		}
	
	co_return FuckassPair{resp, resp.errorMessage()};
}

void ServerManager::authenticate(argon::AccountData data) {
	Notification::create("SerpentLua: Authentication Started", NotificationIcon::Info)->show();

	async::spawn([data = std::move(data)]() -> arc::Future<> {
		auto res = co_await argon::startAuth(data);
		if (res.isOk()) {
			auto web = ServerManager::get()->createReq();
			auto json = matjson::makeObject({
				{"account_id", data.accountId},
				{"argon_token", res.unwrap()}
			});

			web.bodyJSON(json);
			web.header("Content-Type", "application/json");

			auto response = co_await ServerManager::get()->sendReq("POST", "/api/v1/auth/validate", web);

			if (response.ok() && response.string().isOk()) {
				ServerManager::get()->setSessionToken(response.string().unwrap());
				geode::queueInMainThread([](){ Notification::create("SerpentLua: Authenticated successfully!", NotificationIcon::Success)->show(); });
			} else {
				geode::queueInMainThread([](){ Notification::create("SerpentLua: Server Authentication failed.", NotificationIcon::Error)->show(); });
			}
			co_return;
		} else {
			geode::queueInMainThread([]() { Notification::create("SerpentLua: Argon Authentication failed", NotificationIcon::Error)->show(); });
		}
	});
}