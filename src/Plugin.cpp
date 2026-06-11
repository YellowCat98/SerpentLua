#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

void Plugin::terminate() {
	if (native) {
		if (hDll.has_value()) {
			FreeLibrary(hDll.value());
		}
	}
	auto annoyingNoDiscardWarningFix = internal::RuntimeManager::get()->removeLoadedPlugin(this->metadata->id);
	if (annoyingNoDiscardWarningFix.isErr()) {
		log::warn("{}", annoyingNoDiscardWarningFix.err().value());
		return;
	}
	delete this;
}

std::function<void(lua_State*)> Plugin::getEntry() {
	return entry;
}

void Plugin::setPlugin() {
	return internal::RuntimeManager::get()->setPlugin(this);
}

void Plugin::SerpentLuaAPIImpl::log(__metadata m, const char* msg, const char* type) {
	if (std::strcmp(type, "info") == 0) log::info("[PLUGIN] [{}]: {}", m.name, msg);
	else if (strcmp(type, "warn") == 0) log::warn("[PLUGIN] [{}]: {}", m.name, msg);
	else if (strcmp(type, "error") == 0) log::error("[PLUGIN] [{}]: {}", m.name, msg);
	else if (strcmp(type, "debug") == 0) log::debug("[PLUGIN] [{}]: {}", m.name, msg);
	else log::trace("[PLUGIN] [{}]: {}", m.name, msg); // always default to trace if type isnt those things
}

Plugin::__metadata Plugin::SerpentLuaAPIImpl::get_script(lua_State* L) {
	auto script = SerpentLua::internal::RuntimeManager::get()->getScriptByState(L);

	Plugin::__metadata ret;
	ret.name = script->name.c_str();
	ret.developer = script->developer.c_str();
	ret.id = script->id.c_str();
	ret.version = script->version.c_str();
	ret.serpentVersion = script->serpentVersion.c_str();

	std::vector<const char*> plugins;
	plugins.reserve(script->plugins.size());
	for (const auto& buttplug : script->plugins) {
		plugins.push_back(buttplug.c_str());
	}
	ret.pluginsSize = plugins.size();
	ret.plugins = plugins.data();

	return ret;
}

geode::Result<Plugin*, std::string> Plugin::createNative(const std::filesystem::path& path) {
	log::info("Loading Native Plugin {}: initialized", path.filename());
	if (!Mod::get()->getSavedValue<bool>(fmt::format("safe-{}", path.stem())) && !Mod::get()->getSettingValue<bool>("dev-mode")) return Err("Native Plugin {} was imported manually.\nThis plugin will not load unless it's imported through the plugin importer in-game.", path.stem());
	auto configDir = Mod::get()->getConfigDir();
	bool depsDir = std::filesystem::exists(configDir/"plugin_deps"/path.filename());
	if (depsDir) {
		log::warn("Plugin {}: Dynamically linking libraries via the plugin_deps directory is deprecated. This feature will be removed in SerpentLua v2.0.0.", path.filename());
	}
	DLL_DIRECTORY_COOKIE cookie1;
	if (depsDir) cookie1 = AddDllDirectory((configDir/"plugin_deps"/path.filename()).c_str());
	
	DLL_DIRECTORY_COOKIE cookie2 = AddDllDirectory((configDir/"plugin_global_deps").c_str());

	auto temphDll = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (!temphDll) {
		return Err("Plugin {}: Failed to load SLP.", path.filename());
	}

	log::debug("Plugin {}: SLP loaded, gathering metadata...", path.filename());

	auto metaRes = PluginMetadata::createFromSLP(path, temphDll);
	if (metaRes.isErr()) return Err("Gathering Metadata: {}", metaRes.unwrapErr());
	auto metadata = metaRes.unwrap();

	FreeLibrary(temphDll);

	log::debug("Plugin {}: Metadata gathered, executing...", path.filename());


	auto hDll = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);

	if (depsDir) RemoveDllDirectory(cookie1);
	RemoveDllDirectory(cookie2);

	if (!hDll) return Err("Plugin {}: Failed to load with error {}", path.filename(), GetLastError());

	// now we... DONT handle METADATA? it has already retrieved metadata. now we.. handle API FUNCTIONS...!
	log::debug("Plugin {}: SLP executed.", path.filename());

	auto INITFUCKINGAPI = reinterpret_cast<void(*)(Plugin::SerpentLuaAPI)>(GetProcAddress(hDll, "initNativeAPI"));
	if (!INITFUCKINGAPI) {
		FreeLibrary(hDll);
		// return Err("THE FUCKING FUNCTION WASNT FOUND")
		return Err("Plugin {}: initNativeAPI function was not found.", path.filename());
	}

	Plugin::__metadata __md;
	__md.name = metadata->name.c_str();
	__md.developer = metadata->developer.c_str();
	__md.id = metadata->id.c_str();
	__md.version = metadata->version.c_str();
	__md.serpentVersion = metadata->serpentVersion.c_str();
	__md.plugins = {}; // just initializing it to empty to be safe

	Plugin::SerpentLuaAPI API;
	API.handle = hDll;
	API.metadata = __md;
	API.log = &Plugin::SerpentLuaAPIImpl::log;
	API.get_script = &Plugin::SerpentLuaAPIImpl::get_script;

	INITFUCKINGAPI(API);

	auto rawEntry = reinterpret_cast<void(*)(lua_State*)>(GetProcAddress(hDll, "entry"));
	if (!rawEntry) {
		FreeLibrary(hDll);
		return Err("Plugin {}: Entry function was not found within the SLP.", path.filename());
	}

	auto plugin = Plugin::create(metadata, rawEntry);
	if (plugin.isErr()) {
		return Err("Plugin {}: {}", path.filename(), plugin.err());
	};
	auto unwrapped = plugin.unwrap();

	unwrapped->native = true;
	metadata->native = true;
	metadata->loaded = true;

	unwrapped->hDll = hDll;
	return Ok(unwrapped);
}

Result<Plugin*, std::string> Plugin::create(PluginMetadata* metadata, std::function<void(lua_State*)> entry) {
	log::info("Plugin {} creation: initialized.", metadata->id);
	auto ret = new (std::nothrow) Plugin();
	if (!ret) return Err("Plugin creation: Plugin is nullptr.");
	ret->loadCount = 0;

	ret->metadata = metadata;
	ret->entry = entry;
	return Ok(ret);
}