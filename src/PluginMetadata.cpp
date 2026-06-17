#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;
using namespace SerpentLua::internal;

// gonna need createFromSLP for plugin uploading
geode::Result<PluginMetadata*, std::string> PluginMetadata::createFromSLP(const std::filesystem::path& path, HMODULE module) {
	HRSRC res = FindResource(module, "SERPENTLUA_METADATA", RT_RCDATA);
	if (!res) {
		FreeLibrary(module);
		return Err("Plugin {}: Resource \"SERPENTLUA_METADATA\" was not found.", path.filename());
	}
	HGLOBAL hRes = LoadResource(module, res);
	if (!hRes) {
		FreeLibrary(module);
		return Err("Plugin {}: Failed to load resource (err {})", path.filename(), GetLastError());
	}
	void* rawMeta = LockResource(hRes);
	if (!rawMeta) {
		FreeLibrary(module);
		return Err("Plugin {}: Failed to lock resource.", path.filename());
	}
	DWORD size = SizeofResource(module, res);
	if (size == 0) {
		FreeLibrary(module);
		return Err("Plugin {}: Resource size is equal to 0.", path.filename());
	}

	std::string metadataMiniRaw(reinterpret_cast<char*>(rawMeta), size); // this means that its a tiny bit raw, not too much raw. (basically meaning that its in a format thats too hard to understand)

	std::vector<std::string> lines;
	std::string line;
	std::stringstream ss(metadataMiniRaw);

	while (std::getline(ss, line, '\n')) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}
		lines.push_back(line);
	}

	std::map<std::string, std::string> metadataMap;

	for (auto& line : lines) {
		auto pair = ScriptMetadata::createPair(line);
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

	if (string::pathToString(path.stem()) != metadataMap.at("id")) return Err("Plugin {}: ID must match the plugin file name without the `.slp` extension.", path.filename());

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

