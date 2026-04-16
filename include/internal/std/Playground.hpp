#pragma once

#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Playground {
	sol::table entry(sol::state_view state); 

	geode::Result<std::filesystem::path> resolvePath(const std::string& path);
	geode::Result<std::string> reverseResolvePath(const std::string& path);
	geode::Result<void, std::string> DidHeSayThatHisLastNameIsBurger(const std::filesystem::path& path);

	inline std::unordered_map<std::string, std::filesystem::path> schemes;
	struct File final {
		static File create(const std::string& path, bool readOnly);

		std::string read();
		void write(const std::string& what);
		void append(const std::string& what);
		int erase();

		bool hasErrs();
		std::string getErr(int index); // yes this index starts at 1 so that we stay faithful to lua's shitty shit
		int errSize();

		std::string getPath();
		std::string getName(bool withExtension);
	private:
		std::vector<std::string> errs;
		std::string path; // i really dont want to wrap sol2 around std::filesystem::path
		bool readOnly;
		bool failed;
	};

	// Represents a directory pretty much!
	// naming it Folder instead of Dir because thats what roblox uses!
	struct Folder final {
	public:
		static Folder create(const std::string& path);

		sol::table items(sol::this_state ts);

		bool exists(const std::string& item);

		int erase();

		bool hasErrs();
		std::string getErr(int index);
		int errSize();

		std::string getPath();
		std::string getName();

	private:
		std::vector<std::string> errs;
		bool failed;
		std::string path;
	};

	struct exposedFunctions {
		static void init(sol::this_state ts);
	};
}