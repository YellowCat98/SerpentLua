#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;
using namespace SerpentLua::internal;

// gonna need createFromSLP for plugin uploading
geode::Result<PluginMetadata*, std::string> PluginMetadata::createFromScript(const std::filesystem::path& path, bool enforceSameID) {

	std::ifstream file(path / "init.lua");
	if (!file.is_open()) return Err("Plugin `{}`: an error occured opening file.", path.filename());

	std::vector<std::string> lines;
	std::string line;
	size_t count = 0;
	size_t max = 5;

	while (std::getline(file, line) && count < max) {
		lines.push_back(line);
		++count;
	}

	std::map<std::string, std::string> metadataMap;

	for (auto& line : lines) {
		auto pair = utility::parseMetadataEntry(line);
		if (pair == std::pair<std::string, std::string>({})) return Err("Plugin {}: Invalid metadata.", path.filename());
		if (metadataMap.contains(pair.first)) log::warn("Plugin {}: Metadata already contains {}, skipping.", path.filename(), pair.first);
		metadataMap.insert(pair);
	}

	// checking for metadata validity
	std::vector<std::string> requiredKeys = {"name", "id", "version", "serpent-version", "developer"};

	for (auto& req : requiredKeys) {
		if (!metadataMap.contains(req)) return Err("Plugin {}: Metadata is missing `{}` key.", path.filename(), req);
	}

	// repurposes requiredKeys to check for unknown keys
	for (const auto& [key, value] : metadataMap) { // this also disallows things like `--@  developer hello`
		auto it = std::find(requiredKeys.begin(), requiredKeys.end(), key);
		if (it == requiredKeys.end()) return Err("Plugin {}: Unknown Metadata key: {}", path.filename(), key);
	}

	if ((string::pathToString(path.filename()) != metadataMap.at("id")) && enforceSameID) return Err("Plugin {}: ID must match the plugin file name without the `.slp` extension.", path.filename());

	auto verRes = utility::handleVersion(metadataMap.at("version"));
	if (verRes.isErr()) return Err("Plugin {}: Version cannot be parsed: {}", metadataMap.at("id"), *(verRes.err()));

	metadataMap["version"] = verRes.unwrap();

	auto serpVerRes = utility::handleVersion(metadataMap.at("serpent-version"));
	if (serpVerRes.isErr()) return Err("Plugin {}: Serpent version cannot be parsed: {}", metadataMap.at("id"), *(serpVerRes.err()));

	metadataMap["serpent-version"] = serpVerRes.unwrap();

	auto metadata = PluginMetadata::create(metadataMap);
	metadata->path = utils::string::pathToString(path);

	return Ok(metadata);
}

PluginMetadata* PluginMetadata::createFromMod(Mod* mod) {
	auto ret = new (std::nothrow) PluginMetadata();
	if (!ret) return nullptr;
	ret->name = mod->getName();
	ret->developer = mod->getDevelopers()[0];
	ret->id = mod->getID();
	ret->version = mod->getVersion().toVString();
	ret->serpentVersion = Mod::get()->getVersion().toNonVString();

	return ret;
}

PluginMetadata* PluginMetadata::create(std::map<std::string, std::string>& metadata) {
	auto ret = new (std::nothrow) PluginMetadata();
	if (!ret) return nullptr;
	ret->name = metadata["name"];
	ret->developer = metadata["developer"];
	ret->id = metadata["id"];
	ret->version = metadata["version"];
	ret->serpentVersion = metadata["serpent-version"];
	return ret;
}

