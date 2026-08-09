#pragma once

#include <SerpentLua.hpp>
#include <internal/DisplayInfo.hpp>

namespace SerpentLua::internal {
	class ServerManager {
	public:

		// written in order of hierarchy
		enum class Status {
			Unknown,
			Banned,
			Peasant,
			Verified,
			Staff,
			Admin,
			Owner
		};

		static ServerManager* get();

		void setServerUrl(const std::string& url);
		void setSessionToken(const std::string& token);

		std::string getEndpoint(const std::string& path);

		bool isAuthenticated();

		arc::Future<std::string> setStatus();
		ServerManager::Status getStatusCached();
		std::vector<ServerManager::Status> getStatusSettables(ServerManager::Status status);
		std::vector<ServerManager::Status> getStatusSettables();
		std::string getBanReason();
		bool resolveStatus(ServerManager::Status status, const ServerManager::Status& other);
		bool resolveStatus(const ServerManager::Status& other);
		std::string statusString(ServerManager::Status status, bool technical = false);
		std::string statusString(bool technical = false);

		geode::utils::web::WebRequest createReq(bool withAuth = false); // withAuth just passes in the session token as Authorization lol!

		arc::Future<geode::utils::web::WebResponse> sendReq(std::string method, std::string path, geode::utils::web::WebRequest req);
		arc::Future<geode::Result<geode::utils::web::WebResponse>> downloadPlugin(bool script, const DisplayInfo& info, ButtonSprite* button = nullptr);
		void authenticate(argon::AccountData data);

		arc::Future<geode::Result<matjson::Value>> getIndexJSON(std::string repo, std::string tag);
		arc::Future<geode::Result<std::string>> getDownloadHashByUrl(std::string url);
		std::string getDownloadHashByData(std::vector<uint8_t> data);

		arc::Future<geode::Result<std::pair<PluginMetadata*, geode::utils::web::WebResponse>>> getPluginMetadataByUrl(std::string url);
	private:
		std::string sessionToken;
		std::string url;
		ServerManager::Status status;
		std::string banReason; // would be empty if the user isnt banned
	};
}