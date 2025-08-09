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
    if (!Mod::get()->getSavedValue<bool>(fmt::format("safe-{}", path.stem())) && !Mod::get()->getSettingValue<bool>("dev-mode")) return Err("Native Plugin {} was imported manually.\nThis plugin will not load unless it's imported through the plugin importer in-game.", path.stem());
    auto configDir = Mod::get()->getConfigDir();
    bool depsDir = std::filesystem::exists(configDir/"plugin_deps"/path.filename().string());
    if (!depsDir) {
        log::warn("Plugin {}: No dependencies directory found, will continue loading regardless.", path.filename());
    }
    DLL_DIRECTORY_COOKIE cookie1;
    if (depsDir) cookie1 = AddDllDirectory((configDir/"plugin_deps"/path.filename().string()).wstring().c_str());
    
    DLL_DIRECTORY_COOKIE cookie2 = AddDllDirectory((configDir/"plugin_global_deps").wstring().c_str());

    auto hDll = LoadLibraryExA(path.string().c_str(), NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);

    if (depsDir) RemoveDllDirectory(cookie1);
    RemoveDllDirectory(cookie2);

    if (!hDll) return Err("Plugin {}: Failed to load with error {}", path.filename(), GetLastError());

    // now we handle METADATA.
    log::debug("Plugin {}: DLL loaded, gathering metadata...", path.filename());

    auto rawMeta = reinterpret_cast<const Plugin::__metadata*>(GetProcAddress(hDll, "plugin_metadata"));
    if (!rawMeta) {
        FreeLibrary(hDll); // i shouldve probably did this before
        return Err("Plugin {}: No metadata was found within the DLL.", path.filename());
    }
    log::debug("Native DLL info: \n{}\n{}\n{}\n{}", rawMeta->name,rawMeta->id,rawMeta->version,rawMeta->serpentVersion);
    std::map<std::string, std::string> mapMetadata = {
        {"name", std::string(rawMeta->name)},
        {"developer", std::string(rawMeta->developer)},
        {"id", std::string(rawMeta->id)},
        {"version", std::string(rawMeta->version)},
        {"serpent-version", std::string(rawMeta->serpentVersion)}
    };
    auto metadata = PluginMetadata::create(mapMetadata);

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
    log::info("Plugin creation: initialized.");
    auto ret = new Plugin();
    if (!ret) return Err("Plugin creation: Plugin is nullptr.");
    ret->native = false;
    ret->loadedSomewhere = false; // assume false until reassigned.

    ret->metadata = metadata;
    ret->entry = entry;
    return Ok(ret);
}