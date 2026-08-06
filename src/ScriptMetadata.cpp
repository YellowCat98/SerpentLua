#include <internal/SerpentLua.hpp>
#include <Geode/Geode.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void SerpentLua::ScriptMetadata::setPlugins() {
	if (this->pluginIDstring.empty()) return;
	std::vector<std::pair<std::string, std::string>> pluginIDs;

	std::istringstream iss(this->pluginIDstring);
	std::string plugin;

	while (iss >> plugin) {
		std::string before;
		std::string after;

		auto pos = plugin.find('@');
		if (pos != std::string::npos) {
			before = plugin.substr(0, pos);
			after = plugin.substr(pos + 1);
		} else {
			before = plugin;
			after = "*";
		}

		pluginIDs.push_back({before, after});
	}

	this->plugins = pluginIDs;
}

geode::Result<SerpentLua::ScriptMetadata*, std::string> SerpentLua::ScriptMetadata::getScriptByID(const std::string& id) {
	return SerpentLua::internal::RuntimeManager::get()->getScriptByID(id);
}

SerpentLua::ScriptMetadata* SerpentLua::ScriptMetadata::getScriptByState(lua_State* L) {
	return SerpentLua::internal::RuntimeManager::get()->getScriptByState(L);
}

geode::Result<SerpentLua::ScriptMetadata*, std::string> SerpentLua::ScriptMetadata::createFromScript(const std::filesystem::path& scriptPath) {
	if (!std::filesystem::exists(scriptPath)) return Err("Script {} doesn't exist.", scriptPath.filename());
	log::info("Script `{}` creation: Initialized.", scriptPath.filename());
	std::ifstream file(scriptPath);
	if (!file.is_open()) return Err("Script `{}` creation: an error occured opening file.", scriptPath.filename());

	std::vector<std::string> lines;
	std::string line;
	size_t count = 0;
	size_t max = 7;

	while (std::getline(file, line) && count < max) {
		lines.push_back(line);
		++count;
	}

	// now to PARSING
	std::map<std::string, std::string> metadata; // turn the lines into a more readable format
	for (auto& line : lines) {
		if (std::all_of(line.begin(), line.end(), [](unsigned char c) {
			return std::isspace(c);
		})) continue;
		auto pair = utility::parseMetadataEntry(line);
		if (pair == std::pair<std::string, std::string>({})) return Err("Script `{}` metadata: The first {} lines in a script must be metadata.", scriptPath.filename(), max+1);

		if (metadata.contains(pair.first)) return Err("Script `{}` metadata: Metadata already contains key {}", scriptPath.filename(), pair.first);

		metadata.insert(pair);
	}
	log::debug("Script `{}` metadata: Retrieved metadata, checking metadata validity...", scriptPath.filename());


	// checking for metadata validity
	std::vector<std::string> requiredKeys = {"name", "id", "version", "serpent-version", "developer"};

	for (auto& req : requiredKeys) {
		if (!metadata.contains(req)) return Err("Script `{}` metadata: Metadata is missing `{}` key.", scriptPath.filename(), req);
	}

	requiredKeys.push_back("nostd"); // nostd is not required, im only adding nostd to it because requiredKeys is now repurposed to check for unknown keys!
	requiredKeys.push_back("plugins"); // same for this one
	for (const auto& [key, value] : metadata) { // this also disallows things like `--@  developer hello`
		auto it = std::find(requiredKeys.begin(), requiredKeys.end(), key);
		if (it == requiredKeys.end()) return Err("Script `{}` metadata: Unknown Metadata key: {}", scriptPath.filename(), key);
	}
	
	// HOW the fuck did i forget this holy fucking SHIT.
	metadata.insert({"path", utils::string::pathToString(scriptPath)});

	if (scriptPath.stem() != metadata.at("id")) return Err("Script `{}` metadata: Script filename with the extension omitted must match Script ID. ({} != {})", scriptPath.filename(), scriptPath.stem(), metadata.at("id"));

	auto res = utility::handleVersion(metadata.at("version"));
	if (res.isErr()) return Err("Script `{}` metadata: Version cannot be parsed: {}", metadata.at("id"), *(res.err()));

	metadata["version"] = res.unwrap();

	auto serpVerRes = utility::handleVersion(metadata.at("serpent-version"));
	if (serpVerRes.isErr()) return Err("Plugin {}: Serpent version cannot be parsed: {}", metadata.at("id"), *(serpVerRes.err()));

	metadata["serpent-version"] = serpVerRes.unwrap();

	log::debug("Script `{}` metadata: Metadata gathered.", scriptPath.filename());

	return Ok(ScriptMetadata::create(metadata));
}

SerpentLua::ScriptMetadata* SerpentLua::ScriptMetadata::create(std::map<std::string, std::string>& metadata) {
	auto ret = new (std::nothrow) SerpentLua::ScriptMetadata();
	if (!ret) return nullptr;

	ret->name = metadata["name"];
	ret->id = metadata["id"];
	ret->version = metadata["version"];
	ret->serpentVersion = metadata["serpent-version"];
	ret->nostd = metadata.contains("nostd");
	ret->path = metadata["path"];
	if (metadata.contains("plugins")) ret->pluginIDstring = metadata["plugins"];
	else ret->pluginIDstring = "";
	ret->developer = metadata["developer"];
	ret->setPlugins();

	return ret;
}