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
    return internal::RuntimeManager::get()->setPlugin(this->metadata->id, this);
}

#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
geode::Result<Plugin*, std::string> Plugin::createNative(const std::filesystem::path& path) {
    log::info("Loading Native Plugin {}: initialized", path.filename());
    if (!Mod::get()->getSavedValue<bool>(fmt::format("safe-{}", path.stem())) && !Mod::get()->getSettingValue<bool>("dev-mode") && !Mod::get()->getSavedValue<bool>("should-show-warning")) return Err("Native Plugin {} was imported manually.\nThis plugin will not load unless it's imported through the plugin importer in-game.", path.stem());
    auto configDir = Mod::get()->getConfigDir();
    bool depsDir = std::filesystem::exists(configDir/"plugin_deps"/path.filename().string());
    if (!depsDir) {
        log::warn("Plugin {}: No dependencies directory found, will continue loading regardless.", path.filename());
    }
    DLL_DIRECTORY_COOKIE cookie1;
    if (depsDir) cookie1 = AddDllDirectory((configDir/"plugin_deps"/path.filename().string()).wstring().c_str());
    
    DLL_DIRECTORY_COOKIE cookie2 = AddDllDirectory((configDir/"plugin_global_deps").wstring().c_str());

    //auto hDll = LoadLibraryExA(path.string().c_str(), NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);

    auto temphDll = LoadLibraryExA(path.string().c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!temphDll) {
        return Err("Plugin {}: Failed to load DLL.", path.filename());
    }

    log::debug("Plugin {}: DLL loaded, gathering metadata...", path.filename());

    HRSRC res = FindResource(temphDll, "SERPENTLUA_METADATA", RT_RCDATA);
    if (!res) {
        FreeLibrary(temphDll);
        return Err("Plugin {}: Resource \"SERPENTLUA_METADATA\" was not found.", path.filename());
    }
    HGLOBAL hRes = LoadResource(temphDll, res);
    if (!hRes) {
        FreeLibrary(temphDll);
        return Err("Plugin {}: Failed to load resource (err {})", path.filename(), GetLastError());
    }
    void* rawMeta = LockResource(hRes);
    if (!rawMeta) {
        FreeLibrary(temphDll);
        return Err("Plugin {}: Failed to lock resource.", path.filename());
    }
    DWORD size = SizeofResource(temphDll, res);
    if (size == 0) {
        FreeLibrary(temphDll);
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

    auto metadata = PluginMetadata::create(metadataMap);

    FreeLibrary(temphDll);

    log::debug("Plugin {}: Metadata gathered, executing...", path.filename());


    auto hDll = LoadLibraryExA(path.string().c_str(), NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);

    if (depsDir) RemoveDllDirectory(cookie1);
    RemoveDllDirectory(cookie2);

    //if (!hDll) return Err("Plugin {}: Failed to load with error {}", path.filename(), GetLastError());

    // now we handle METADATA.
    log::debug("Plugin {}: DLL executed.", path.filename());

    auto rawEntry = reinterpret_cast<void(*)(lua_State*)>(GetProcAddress(hDll, "entry"));
    if (!rawEntry) {
        FreeLibrary(hDll);
        return Err("Plugin {}: Entry function was not found within the DLL.", path.filename());
    }

    auto plugin = Plugin::create(metadata, rawEntry);
    if (plugin.isErr()) {
        return Err("Plugin {}: {}", path.filename(), plugin.err());
    };
    auto unwrapped = plugin.unwrap();

    unwrapped->native = true;

    unwrapped->hDll = hDll;
    return Ok(unwrapped);
}
#endif

Result<Plugin*, std::string> Plugin::create(PluginMetadata* metadata, std::function<void(lua_State*)> entry) {
    log::info("Plugin {} creation: initialized.", metadata->id);
    auto ret = new Plugin();
    if (!ret) return Err("Plugin creation: Plugin is nullptr.");
    ret->loadCount = 0;

    ret->metadata = metadata;
    ret->entry = entry;
    return Ok(ret);
}