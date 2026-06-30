#include <SerpentLua.hpp>
#include <internal/SerpentLua.hpp>
#include <libs/picosha2/picosha2.h>

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

arc::Future<web::WebResponse> ServerManager::sendReq(std::string method, std::string path, web::WebRequest req) {
	auto url = this->getEndpoint(path);
	co_return co_await req.send(method, url);
}

arc::Future<geode::Result<geode::utils::web::WebResponse>> ServerManager::downloadPlugin(bool script, const DisplayInfo& info, ButtonSprite* button) {
	using FuckassPair = std::pair<web::WebResponse, std::string>;
	if (info.scriptExample.empty() && script) co_return Err("Plugin has no example.");

	auto req = this->createReq(false);
	req.param("id", info.id);
	req.param("script", static_cast<int>(script));
	std::string originalBtnStr; 
	if (button) {
		req.onProgress([button](const web::WebProgress& prog) {
			if (button) {
				auto downloadProg = prog.downloadProgress().value_or(0.0f);
				button->setString(fmt::format("Prog: {:.2f}%", downloadProg).c_str());
			}
		});
	}

	auto originalHash = script ? info.scriptDownloadHash : info.downloadHash;

	auto resp = co_await this->sendReq("GET", "/api/v1/plugin/download", req);
	
	if (resp.ok()) {
		// first tings first we check da hash
		// https://github.com/geode-sdk/geode/blob/f60a77bdf41ba5974dbd1aa2368e5db9cae4a3b2/loader/hash/hash.cpp#L29
		std::vector<uint8_t> hashVec(picosha2::k_digest_size);
		picosha2::hash256(resp.data().begin(), resp.data().end(), hashVec);
		auto hash = fmt::format("sha256:{}", picosha2::bytes_to_hex_string(hashVec.begin(), hashVec.end()));
		if (originalHash != hash) Err("Hash mismatch");

		auto dir = Mod::get()->getConfigDir() / "pending_install" / (script ? "scripts" : "plugins");
		auto createdRes = file::createDirectoryAll(dir);
		if (createdRes.isErr()) {
			co_return Err(createdRes.unwrapErr());
		}
		auto final = dir / (script ? info.scriptFilename : info.filename);
		if (std::filesystem::exists(final)) {
			std::filesystem::remove_all(final);
		}
		auto res = resp.into(final);
		if (res.isErr()) {
			co_return Err(res.unwrapErr());
		}

		co_return Ok(resp);
	}
	
	co_return Err(resp.string().unwrapOr("Unknown Error."));
}

void ServerManager::authenticate(argon::AccountData data) {
	log::info("Authentication started.");

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
				auto err = co_await ServerManager::get()->setStatus();
				if (!err.empty()) log::warn("Unable to get user status, Dashboard cannot be accessed.\nError: {}", err);
				geode::queueInMainThread([](){ Notification::create("SerpentLua: Authenticated successfully!", NotificationIcon::Success)->show(); });
			} else {
				geode::queueInMainThread([](){ Notification::create("SerpentLua: Server Authentication failed.", NotificationIcon::Error)->show(); });
			}
			co_return;
		} else {
			log::error("Failed to authenticate with Argon: {}", res.unwrapErr());
			geode::queueInMainThread([]() { Notification::create("SerpentLua: Argon Authentication failed", NotificationIcon::Error)->show(); });
		}
	});
}

bool ServerManager::isAuthenticated() {
	return !sessionToken.empty();
}

// tiny helper function
ServerManager::Status statusFromString(const std::string& str) {
	if (str == "banned") return ServerManager::Status::Banned;
	if (str.empty()) return ServerManager::Status::Peasant;
	if (str == "verified") return ServerManager::Status::Verified;
	if (str == "staff") return ServerManager::Status::Staff;
	if (str == "admin") return ServerManager::Status::Admin;
	if (str == "owner") return ServerManager::Status::Owner;
	return ServerManager::Status::Unknown;
}

arc::Future<std::string> ServerManager::setStatus() {
	using FUCKINGPAIR = std::pair<std::string, ServerManager::Status>;

	auto req = this->createReq(false);
	req.param("account_id", GJAccountManager::get()->m_accountID);

	auto resp = co_await this->sendReq("GET", "/api/v1/user/status", req);

	if (!(resp.code() >= 200 && resp.code() < 300)) co_return resp.string().unwrap();

	auto jsonRes = resp.json();
	if (jsonRes.isErr()) co_return jsonRes.unwrapErr();

	auto json = jsonRes.unwrap();
	// ok got it

	auto statusStrRes = json["status"].asString();
	if (statusStrRes.isErr()) status = ServerManager::Status::Peasant;
	else {
		status = statusFromString(statusStrRes.unwrap());

		if (status == ServerManager::Status::Banned) {
			auto banReasonRes = json["ban_reason"].asString();
			if (!banReasonRes.isErr()) banReason = banReasonRes.unwrap();
		}
	}

	co_return "";
}

ServerManager::Status ServerManager::getStatusCached() {
	return status;
}

std::string ServerManager::getBanReason() {
	return banReason;
}

bool ServerManager::resolveStatus(const ServerManager::Status& other) {
	return this->resolveStatus(status, other);
}

bool ServerManager::resolveStatus(ServerManager::Status status, const ServerManager::Status& other) {
	int numStatus = static_cast<int>(status);
	int numOther = static_cast<int>(other);

	return numStatus >= numOther;
}

std::string ServerManager::statusString() {
	return this->statusString(this->status);
}

std::string ServerManager::statusString(ServerManager::Status status) {
	switch (status) {
		case ServerManager::Status::Unknown: return "Unknown";
		case ServerManager::Status::Banned: return "Banned";
		case ServerManager::Status::Peasant: return "User"; // i dont think i should really make the average user named Peasant
		case ServerManager::Status::Verified: return "Verified";
		case ServerManager::Status::Staff: return "Staff";
		case ServerManager::Status::Admin: return "Admin";
		case ServerManager::Status::Owner: return "Owner";
	}
}

std::vector<ServerManager::Status> ServerManager::getStatusSettables() {
	return getStatusSettables(this->status);
}

std::vector<ServerManager::Status> ServerManager::getStatusSettables(ServerManager::Status status) {
	switch (status) {
		case ServerManager::Status::Owner:
			return {
				ServerManager::Status::Banned,
				ServerManager::Status::Peasant,
				ServerManager::Status::Verified,
				ServerManager::Status::Staff,
				ServerManager::Status::Admin,
				ServerManager::Status::Owner
			};

		case ServerManager::Status::Admin:
			return {
				ServerManager::Status::Banned,
				ServerManager::Status::Verified
			};

		case ServerManager::Status::Staff:
			return {
				ServerManager::Status::Banned
			};

		case ServerManager::Status::Unknown:
		case ServerManager::Status::Peasant:
		case ServerManager::Status::Verified:
		case ServerManager::Status::Banned:
		default:
			return {};
	}
}

arc::Future<geode::Result<matjson::Value>> ServerManager::getIndexJSON(std::string repo, std::string tag) {
	auto req = web::WebRequest();
	req.userAgent("cpp-client");
	req.header("Accept", "application/vnd.github+json");
	req.header("X-GitHub-Api-Version", "2026-03-10");

	std::string apiUrl = fmt::format("https://api.github.com/repos/{}/releases/tags/{}", repo, tag);
	// really didnt want to force github but eh who cares

	auto resp = co_await req.get(apiUrl);

	auto jsonRes = resp.json();
	if (jsonRes.isErr()) {
		Err(jsonRes.unwrapErr());
	}

	auto json = jsonRes.unwrap();

	if (!resp.ok()) {
		auto msgRes = json["message"].asString();

		if (msgRes.isOk()) {
			Err(json["message"].asString().unwrap());
		} else {
			Err("Unknown error.");
		}
	}

	auto assets = json["assets"].asArray().unwrap();

	std::string downloadUrl;
	for (const auto& index : assets) {
		if (index["name"].asString().unwrap() == "index.json") {
			downloadUrl = index["browser_download_url"].asString().unwrap();
			break;
		}
	}

	if (downloadUrl.empty()) {
		Err("Unable to find asset `index.json`.");
	}

	auto downReq = web::WebRequest();

	auto downResp = co_await downReq.get(downloadUrl);

	if (!downResp.ok()) {
		Err("index.json download failed (Code {})", downResp.code());
	}

	auto indexJSON = downResp.json();

	if (indexJSON.isErr()) {
		co_return Err(indexJSON.unwrapErr());
	}

	co_return Ok(indexJSON.unwrap());
}

arc::Future<geode::Result<std::string>> ServerManager::getDownloadHashByUrl(std::string url) {
	auto req = web::WebRequest();

	auto resp = co_await req.get(url);
	if (!resp.ok()) {
		co_return Err("Unable to download file (Code {})", resp.code());
	}

	std::vector<uint8_t> hashVec(picosha2::k_digest_size);
	picosha2::hash256(resp.data().begin(), resp.data().end(), hashVec);
	co_return Ok(fmt::format("sha256:{}", picosha2::bytes_to_hex_string(hashVec.begin(), hashVec.end())));
}

std::string ServerManager::getDownloadHashByData(std::vector<uint8_t> data) {
	std::vector<uint8_t> hashVec(picosha2::k_digest_size);
	picosha2::hash256(data.begin(), data.end(), hashVec);
	return fmt::format("sha256:{}", picosha2::bytes_to_hex_string(hashVec.begin(), hashVec.end()));
}

arc::Future<geode::Result<std::pair<PluginMetadata*, geode::utils::web::WebResponse>>> ServerManager::getPluginMetadataByUrl(std::string url) {
	auto req = web::WebRequest();

	auto resp = co_await req.get(url);
	if (!resp.ok()) {
		co_return Err("Unable to download file (Code {})", resp.code());
	}

	auto path = Mod::get()->getConfigDir()/"temp"/"__tempPulgin.boobies";

	auto moveIntoRes = resp.into(path);

	if (moveIntoRes.isErr()) {
		co_return Err(moveIntoRes.unwrapErr());
	}

	auto hDll = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (!hDll) {
		std::filesystem::remove(path);
		co_return Err("Failed to get metadata: Couldn't load plugin");
	}

	auto metaRes = PluginMetadata::createFromSLP(path, hDll, false);

	if (metaRes.isErr()) {
		std::filesystem::remove(path);
		co_return Err(metaRes.unwrapErr());
	}

	std::filesystem::remove(path);

	co_return Ok(std::pair<PluginMetadata*, geode::utils::web::WebResponse>{metaRes.unwrap(), resp});
}