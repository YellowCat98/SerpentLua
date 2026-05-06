#include "Geode/utils/web.hpp"
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

	req.timeout(std::chrono::seconds(60));

	if (withAuth) {
		req.header("Authorization", this->sessionToken);
	}

	return req;
}

void ServerManager::sendReq(geode::async::TaskHolder<geode::utils::web::WebResponse>& listener, const std::string& method, const std::string& path, web::WebRequest& req, std::function<void(geode::utils::web::WebResponse)> lambda) {
	listener.spawn(
		std::move(req.send(method, this->getEndpoint(path))),
		lambda
	);
}