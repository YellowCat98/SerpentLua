#include "Geode/utils/file.hpp"
#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

void Plugin::terminate() {
	(void)internal::RuntimeManager::get()->removeLoadedPlugin(this->metadata->id); // this only returns an error when the plugin isnt registered at all
	if (this->state) lua_close(this->state);
	
	delete this;
}

std::function<bool()> Plugin::getEntry() {
	return entry;
}

std::function<bool(lua_State*)> Plugin::getOnScriptLoaded() {
	return onScriptLoaded;
}

lua_State* Plugin::getLuaState() {
	return state;
}

void Plugin::setPlugin() {
	return internal::RuntimeManager::get()->setPlugin(this);
}

geode::Result<Plugin*, std::string> Plugin::createNative(const std::filesystem::path& path) {
	log::info("Loading Native Plugin {}: initialized", path.filename());
	if (!Mod::get()->getSavedValue<bool>(fmt::format("safe-{}", path.stem())) && !Mod::get()->getSettingValue<bool>("dev-mode")) return Err("Native Plugin {} was imported manually.\nThis plugin will not load unless it's imported through the plugin importer in-game.", path.stem());
	auto configDir = Mod::get()->getConfigDir();
	auto unzipped = configDir / "unzipped" / path.stem();
	bool skipUnzip = false;

	if (std::filesystem::exists(unzipped)) {
		auto originalHashRes = utils::file::readString(unzipped / ".slp_sha256");
		if (originalHashRes.isOk()) {
			auto originalHash = originalHashRes.unwrap();

			auto binaryRes = utils::file::readBinary(path);
			if (binaryRes.isErr()) {
				return Err("Plugin {}: Unable to read SLP: {}", path.filename(), binaryRes.unwrapErr()); // if this fails then i dont think unzipping would work either
			}
			auto binary = binaryRes.unwrap();

			auto hash = internal::utility::sha256(binary);

			skipUnzip = hash == originalHash;
		} else {
			skipUnzip = false;
			log::warn("Plugin {}: Unable to read .slp_sha256: Will unzip anyway.", path.filename());
		}
	}


	log::debug("Plugin {}: Unpacking SLP...", path.filename());

	if (!skipUnzip) {
		log::debug("Plugin {}: Hash mismatch, unzipping.", path.filename());
		std::filesystem::remove_all(unzipped);
		auto unzip = utils::file::Unzip::intoDir(path, unzipped);
		if (unzip.isErr()) return Err("Plugin: Unable to unpack SLP: {}", unzip.unwrapErr());

		auto binary = utils::file::readBinary(path).unwrap(); // if it could unzip then this shouldnt error (hopefully)
		auto hash = internal::utility::sha256(binary);

		auto write = utils::file::writeString(unzipped / ".slp_sha256", hash);
		if (write.isErr()) {
			log::warn("Plugin {}: Couldn't write .slp_sha256: {}. Will continue loading regardless.", path.filename(), write.unwrapErr());
		}
	} else {
		log::debug("Plugin {}: Same hash, skipping unzip.", path.filename());
	}

	auto initRes = utils::file::readString(unzipped / "init.lua");
	if (initRes.isErr()) return Err("Plugin {}: Unable to read init script: {}", path.filename(), initRes.unwrapErr());

	log::debug("Plugin {}: SLP loaded, gathering metadata...", path.filename());

	auto metaRes = PluginMetadata::createFromScript(unzipped);
	if (metaRes.isErr()) return Err("Plugin {}: Gathering Metadata: {}", path.filename(), metaRes.unwrapErr());
	auto metadata = metaRes.unwrap();

	log::debug("Plugin {}: Metadata gathered, executing...", path.filename());

	auto state = luaL_newstate();
	luaL_openlibs(state); // plugins were never sandboxed anyway

	if (luaL_loadfile(state, utils::string::pathToString((unzipped / "init.lua")).c_str()) != LUA_OK) {
		auto err = Err("Plugin {}: Compile error: {}", path.filename(), lua_tostring(state, -1));
		lua_close(state);
		return err;
	}

	if (lua_pcall(state, 0, 1, 0) != LUA_OK) {
		auto err = Err("Plugin {}: Runtime error: {}", path.filename(), lua_tostring(state, -1));
		lua_close(state);
		return err;
	}

	lua_getfield(state, -1, "entry");
	int entryRef = luaL_ref(state, LUA_REGISTRYINDEX);

	std::function<bool()> entry = [state, entryRef]() {
		lua_rawgeti(state, LUA_REGISTRYINDEX, entryRef);

		return lua_pcall(state, 0, 0, 0) == LUA_OK;
	};

	lua_getfield(state, -1, "onScriptLoaded");
	int oslRef = luaL_ref(state, LUA_REGISTRYINDEX);

	std::function<bool(lua_State*)> onScriptLoaded = [state, oslRef](lua_State* scriptState) {
		lua_rawgeti(state, LUA_REGISTRYINDEX, oslRef);
		lua_pushlightuserdata(state, scriptState);

		return lua_pcall(state, 1, 0, 0) == LUA_OK;
	};

	auto plugin = Plugin::create(metadata, state, entry, onScriptLoaded);
	if (plugin.isErr()) {
		return Err("Plugin {}: {}", path.filename(), plugin.err());
	};
	auto unwrapped = plugin.unwrap();

	unwrapped->native = true;
	metadata->native = true;
	metadata->loaded = true;

	return Ok(unwrapped);
}

Result<Plugin*, std::string> Plugin::create(PluginMetadata* metadata, lua_State* state, std::function<bool()> entry, std::function<bool(lua_State*)> onScriptLoaded) {
	log::info("Plugin {}: initialized.", metadata->id);
	auto ret = new (std::nothrow) Plugin();
	if (!ret) return Err("Plugin {}: Not enough memory to create plugin.", metadata->id);
	ret->loadCount = 0;

	ret->state = state;
	ret->metadata = metadata;
	ret->entry = entry;
	ret->onScriptLoaded = onScriptLoaded;

	if (!ret->getEntry()()) {
		auto err = Err("Plugin {}: Entry function errored: {}", metadata->id, lua_tostring(ret->state, -1));
		lua_pop(ret->state, -1);
		ret->terminate();
		return err;
	}

	return Ok(ret);
}