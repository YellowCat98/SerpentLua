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

void ServerManager::downloadPlugin(async::TaskHolder<web::WebResponse>& listener, bool script, const std::string& id, std::function<void(web::WebResponse)> lambda) {
	auto req = this->createReq(false);

	req.param("script", script);
	req.param("id", id);

	this->sendReq(listener, "GET", "/api/v1/plugin/download", req, [lambda, script, id](web::WebResponse resp) {
		if (resp.ok()) {
			auto dir = Mod::get()->getConfigDir() / "pending_install" / (script ? "scripts" : "plugins");
			auto createdRes = file::createDirectoryAll(dir);
			if (createdRes.isErr()) {
				log::error("Unable to install plugin (create dir failed): {}", createdRes.unwrapErr());
				lambda(resp);
				return;
			}
			auto res = resp.into(dir / fmt::format("{}.slp", id));
			if (res.isErr()) {
				log::error("Unable to install plugin: {}", createdRes.unwrapErr());
				lambda(resp);
				return;
			}
		} else {
			log::error("Err: {}", resp.errorMessage());
		}

		lambda(resp);
	});
}