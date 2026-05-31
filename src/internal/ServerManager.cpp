#include <SerpentLua.hpp>
#include <internal/SerpentLua.hpp>
#include <argon/argon.hpp>

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

void ServerManager::sendReq(async::TaskHolder<utils::web::WebResponse>& listener, const std::string& method, const std::string& path, web::WebRequest& req, std::function<void(web::WebResponse)> lambda) {
	listener.spawn(
		std::move(req.send(method, this->getEndpoint(path))),
		lambda
	);
}

void ServerManager::downloadPlugin(async::TaskHolder<web::WebResponse>& listener, bool script, const DisplayInfo& info, std::function<void(web::WebResponse, const std::string&)> lambda) {
	if (info.scriptExample.empty() && script) return;

	auto req = this->createReq(false);

	req.param("script", script);
	req.param("id", info.id);

	this->sendReq(listener, "GET", "/api/v1/plugin/download", req, [lambda, script, info](web::WebResponse resp) {
		if (resp.ok()) {
			auto dir = Mod::get()->getConfigDir() / "pending_install" / (script ? "scripts" : "plugins");
			auto createdRes = file::createDirectoryAll(dir);
			if (createdRes.isErr()) {
				lambda(resp, createdRes.unwrapErr());
				return;
			}
			auto final = dir / (script ? info.scriptFilename : info.filename);
			if (std::filesystem::exists(final)) {
				std::filesystem::remove_all(final);
			}
			auto res = resp.into(dir / (script ? info.scriptFilename : info.filename));
			if (res.isErr()) {
				lambda(resp, res.unwrapErr());
				return;
			}
		} else {
			lambda(resp, std::string(resp.errorMessage()));
			return;
		}

		lambda(resp, "");
	});
}

void ServerManager::authenticate(async::TaskHolder<web::WebResponse>& webListener) {
	Notification::create("SerpentLua: Authentication Started", NotificationIcon::Info)->show();
	async::spawn(
		argon::startAuth(),
		[&webListener](Result<std::string> res) {
			if (res.isOk()) {
				auto web = ServerManager::get()->createReq();
				auto json = matjson::makeObject({
					{"account_id", argon::getGameAccountData().accountId},
					{"argon_token", res.unwrap()}
				});

				web.bodyJSON(json);
				web.header("Content-Type", "application/json");

				ServerManager::get()->sendReq(webListener, "POST", "/api/v1/auth/validate", web,
					[](web::WebResponse response) {
						if (response.ok() && response.string().isOk()) {
							ServerManager::get()->setSessionToken(response.string().unwrap());
							Notification::create("SerpentLua: Authenticated successfully!", NotificationIcon::Success)->show();
						} else {
							Notification::create("SerpentLua: Server Authentication failed.", NotificationIcon::Error)->show();
						}
					}
				);
			} else {
				Notification::create("SerpentLua: Argon Authentication failed", NotificationIcon::Error)->show();
			}
		}
	);
}