#include <SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

std::function<void(lua_State*)> Plugin::getEntry() {
    return entry;
}
#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
geode::Result<Plugin*, std::string> Plugin::createNative(const std::filesystem::path& path) {
    log::info("Loading Plugin {}", path.filename());
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
    if (!rawMeta) return Err("Plugin {}: No metadata was found.", path.filename());
    log::info("\n{}\n{}\n{}\n{}", rawMeta->name,rawMeta->id,rawMeta->version,rawMeta->serpentVersion);
    std::map<std::string, std::string> mapMetadata = {
        {"name", std::string(rawMeta->name)},
        {"id", std::string(rawMeta->id)},
        {"version", std::string(rawMeta->version)},
        {"serpent-version", std::string(rawMeta->serpentVersion)}
    };
    auto metadata = PluginMetadata::create(mapMetadata);

    auto rawEntry = reinterpret_cast<void(*)(lua_State*)>(GetProcAddress(hDll, "entry"));
    if (!rawEntry) return Err("Plugin {}: Entry function was not found.", path.filename());

    auto plugin = Plugin::create(metadata, rawEntry);
    if (plugin.isErr()) return Err("Plugin {}: {}", path.filename(), plugin.err());

    return Ok(plugin.unwrap());
}
#endif

Result<Plugin*, std::string> Plugin::create(PluginMetadata metadata, std::function<void(lua_State*)> entry) {
    auto ret = new Plugin();
    if (!ret) return Err("Plugin creation: Plugin is nullptr.");
    ret->native = false;

    ret->metadata = metadata;
    ret->entry = entry;
    return Ok(ret);
}