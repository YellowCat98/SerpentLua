#include <internal/std/Playground.hpp>
#include <internal/std/PluginEntry.hpp>
#include <sstream>

using namespace SerpentLua::internal;
using namespace geode::prelude;

geode::Result<std::filesystem::path> ScriptBuiltin::Playground::resolvePath(const std::string& path) {
	std::size_t pos = path.find("://");
	if (pos != std::string::npos) {
		std::string before = path.substr(0, pos);
		std::string after = path.substr(pos + 3);

		if (!ScriptBuiltin::Playground::schemes.contains(before)) {
			return Err("Unrecognized scheme: {}", before);
		}

		auto base = ScriptBuiltin::Playground::schemes.at(before);

		auto fullPath = base / after;

		return Ok(fullPath);
	}

	return Err("Invalid playground syntax.");
}

geode::Result<std::string> ScriptBuiltin::Playground::reverseResolvePath(const std::string& path) {
	auto coolerPath = std::filesystem::path(path);

	for (const auto& [k, v] : ScriptBuiltin::Playground::schemes) {
		auto coolerBegin = coolerPath.begin();
		auto coolerEnd = coolerPath.end();

		auto vBegin = v.begin();
		auto vEnd = v.end();

		auto [coolerIt, vIt] = std::mismatch(coolerBegin, coolerEnd, vBegin, vEnd);

		if (vIt != vEnd) continue;

		if (coolerIt != coolerEnd && std::distance(coolerPath.begin(), coolerIt) < std::distance(v.begin(), v.end())) continue;

		std::filesystem::path remainder;
		for (; coolerIt != coolerEnd; ++coolerIt) {
			remainder /= *coolerIt;
		}

		return Ok(fmt::format("{}://{}", k, remainder));
	}

	return Err("Cannot reverse resolve path.");
}

geode::Result<void, std::string> ScriptBuiltin::Playground::DidHeSayThatHisLastNameIsBurger(const std::filesystem::path& path) {
	if (std::filesystem::is_symlink(path)) {
		return Err("playground does not allow symbolic links.");
	}

	for (const auto& part : path) {
		if (part == ".." || part == ".") {
			return Err("playground does not allow `{}`.", part);
		}
	}

	return Ok();
}

ScriptBuiltin::Playground::File ScriptBuiltin::Playground::File::create(const std::string& path, bool readOnly) {
	auto file = ScriptBuiltin::Playground::File();
	file.readOnly = readOnly;


	auto result = ScriptBuiltin::Playground::resolvePath(path);

	if (result.isErr()) {
		file.errs.push_back(*(result.err()));
		file.failed = true;
		return file;
	}

	std::filesystem::path thePath = result.unwrap();
	file.path = utils::string::pathToString(thePath);

	auto check = ScriptBuiltin::Playground::DidHeSayThatHisLastNameIsBurger(thePath);
	if (check.isErr()) {
		file.errs.push_back(*(check.err()));
		file.failed = true;
		return file;
	}

	if (!std::filesystem::exists(thePath)) {
		utils::file::createDirectoryAll(thePath.parent_path()).unwrap();
		std::ofstream the(thePath); // this auto creates it
		if (!the.is_open()) {
			file.failed = true;
			file.errs.push_back("Failed to open file.");
			return file;
		}
		the.close(); // we do not need this file just yet
	}

	return file;
}

bool ScriptBuiltin::Playground::File::hasErrs() {
	return !errs.empty();
}

std::string ScriptBuiltin::Playground::File::getErr(int index) {
	if (index < 1 || index > errs.size()) return std::string("");
	return errs[index - 1];
}

int ScriptBuiltin::Playground::File::errSize() {
	return errs.size();
}

std::string ScriptBuiltin::Playground::File::read() {
	if (failed) return std::string("");
	std::ifstream file(path);
	std::stringstream buffer;

	buffer << file.rdbuf();

	return buffer.str();
}

void ScriptBuiltin::Playground::File::write(const std::string& what) {
	if (failed) return;
	if (readOnly) return; // i am limited by lua's lack of geode::Result and i dont want to wrap around it
	std::ofstream file(path);

	file << what;
}

void ScriptBuiltin::Playground::File::append(const std::string& what) {
	if (failed) return;
	if (readOnly) return;
	// i was just gonna do write(read() + what) but apparently thats bad on memory
	std::ofstream file(path, std::ios::app);

	file << what;
}

int ScriptBuiltin::Playground::File::erase() {
	if (failed) return 1;

	if (!std::filesystem::remove(std::filesystem::path(path))) return 1;

	return 0;
}

std::string ScriptBuiltin::Playground::File::getPath() {
	return path;
}

std::string ScriptBuiltin::Playground::File::getName(bool withExtension) {
	return withExtension ? utils::string::pathToString(std::filesystem::path(path).filename()) : utils::string::pathToString(std::filesystem::path(path).stem());
}

ScriptBuiltin::Playground::Folder ScriptBuiltin::Playground::Folder::create(const std::string& path) {
	auto folder = ScriptBuiltin::Playground::Folder();

	auto result = ScriptBuiltin::Playground::resolvePath(path);

	if (result.isErr()) {
		folder.errs.push_back(*(result.err()));
		folder.failed = true;
		return folder;
	}

	auto thePath = result.unwrap();
	folder.path = utils::string::pathToString(thePath);

	auto check = ScriptBuiltin::Playground::DidHeSayThatHisLastNameIsBurger(thePath);
	if (check.isErr()) {
		folder.errs.push_back(*(check.err()));
		folder.failed = true;
		return folder;
	}

	if (!std::filesystem::exists(thePath)) {
		utils::file::createDirectoryAll(thePath).unwrap();
	}

	return folder;
}

bool ScriptBuiltin::Playground::Folder::exists(const std::string& item) {
	return std::filesystem::exists(std::filesystem::path(path) / item);
}

int ScriptBuiltin::Playground::Folder::erase() {
	if (failed) return 1;

	if (!std::filesystem::remove_all(std::filesystem::path(path))) return 1;

	return 0;
}

// this function passes this_state so we can get the state from it
sol::table ScriptBuiltin::Playground::Folder::items(sol::this_state ts) {
	sol::state_view state(ts);


	auto list = state.create_table();

	int index = 1;
	for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(path))) {
		auto item = entry.path();

		std::string type = std::filesystem::is_directory(item) ? "folder" : "file";

		auto result = ScriptBuiltin::Playground::reverseResolvePath(utils::string::pathToString(item));
		if (result.isErr()) {
			log::error("Failed to reverse resolve path: {}", *(result.err()));
			return state.create_table();
		}

		auto resolved = result.unwrap();

		auto table = state.create_table();
		table["type"] = type;
		table["item"] = resolved;

		list[index++] = table;
	}

	return list;
}

bool ScriptBuiltin::Playground::Folder::hasErrs() {
	return !errs.empty();
}

std::string ScriptBuiltin::Playground::Folder::getErr(int index) {
	if (index < 1 || index > errs.size()) return std::string("");
	return errs[index - 1];
}

int ScriptBuiltin::Playground::Folder::errSize() {
	return errs.size();
}

std::string ScriptBuiltin::Playground::Folder::getPath() {
	return path;
}

std::string ScriptBuiltin::Playground::Folder::getName() {
	return utils::string::pathToString(std::filesystem::path(path).filename());
}

sol::table ScriptBuiltin::Playground::entry(sol::state_view state) {
	auto table = state.create_table();

	table["init"] = [](sol::this_state ts) {
		sol::state_view state(ts);
		lua_State* L = ts;

		sol::table table = sol::stack::get<sol::table>(ts, 1);

		auto scriptDir = Mod::get()->getConfigDir() / "playground" / ScriptBuiltin::getMetadata(L)->id;
		if (!std::filesystem::exists(scriptDir)) utils::file::createDirectoryAll(scriptDir).unwrap();

		auto saveDir = Mod::get()->getSaveDir() / "playground" / ScriptBuiltin::getMetadata(L)->id;
		if (!std::filesystem::exists(saveDir)) utils::file::createDirectoryAll(saveDir).unwrap(); // shut up compiler

		ScriptBuiltin::Playground::schemes = {
			{"script", scriptDir},
			{"user", saveDir}
		};
	}; // creates the stuff if they dont exist and initialize variables

	sol::table file_table = state.create_table();

	file_table.set_function("create", &ScriptBuiltin::Playground::File::create);

	table["File"] = file_table;

	state.new_usertype<ScriptBuiltin::Playground::File>("FileInstance", sol::no_constructor,
		"read", &File::read,
		"erase", &File::erase,
		"append", &File::append, // it looks like a staircase!
		"write", &File::write,
		"hasErrs", &File::hasErrs, 
		"errSize", &File::errSize,
		"getErr", &File::getErr,
		"getPath", &File::getPath,
		"getName", sol::overload(
			[](File& self, bool withExtension) {
				return self.getName(withExtension);
			},
			[](File& self) {
				return self.getName(true);
			}
		)
	);

	sol::table folder_table = state.create_table();

	folder_table.set_function("create", &ScriptBuiltin::Playground::Folder::create);

	table["Folder"] = folder_table;

	state.new_usertype<ScriptBuiltin::Playground::Folder>("FolderInstance", sol::no_constructor,
		"items", &Folder::items,
		"exists", &Folder::exists,
		"hasErrs", &Folder::hasErrs, // it looks like a staircase again!
		"getErr", &Folder::getErr,
		"errSize", &Folder::errSize,
		"erase", &Folder::erase,
		"getPath", &Folder::getPath,
		"getName", &Folder::getName
	);

	return table;
}