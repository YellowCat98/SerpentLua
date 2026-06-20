#include <internal/SerpentLua.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

DisplayInfo DisplayInfo::create(matjson::Value map) {
    DisplayInfo info;

    info.name = map["name"].asString().unwrapOr("");
    info.developer = map["developer"].asString().unwrapOr("");
    info.id = map["id"].asString().unwrapOr("");
    info.version = map["version"].asString().unwrapOr("");
    info.serpentVersion = map["serpent_version"].asString().unwrapOr("");
    info.description = map["description"].asString().unwrapOr("");

    info.downloadLink = map["download_link"].asString().unwrapOr("");
    info.scriptExample = map["script_example"].asString().unwrapOr("");
	info.downloadHash = map["download_hash"].asString().unwrapOr("");
	info.scriptDownloadHash = map["script_download_hash"].asString().unwrapOr("");
	info.filename = map["filename"].asString().unwrapOr("");
	info.scriptFilename = map["script_filename"].asString().unwrapOr("");

    info.source = map["source"].asString().unwrapOr("");
	info.status = map["status"].asString().unwrapOr("");

    info.downloadCount = map["download_count"].asInt().unwrapOr(0);
    info.accountId = map["account_id"].asInt().unwrapOr(0);

    info.releaseDate = map["release_date"].asInt().unwrapOr(0);
    info.lastUpdateDate = map["last_update_date"].asInt().unwrapOr(0);

    info.featured = map["featured"].asBool().unwrapOr(false);

    info.native = false; // you arent really meant to use this to construct local plugins/scripts
    info.loaded = false;
	info.script = false;

    return info;
}

DisplayInfo DisplayInfo::createFromScript(void* script, bool isScript) {
	DisplayInfo info;

	if (isScript) {
		auto theScript = static_cast<ScriptMetadata*>(script);

		info.name = theScript->name;
		info.id = theScript->id;
		info.developer = theScript->developer;
		info.version = theScript->version;
		info.serpentVersion = theScript->serpentVersion;
		info.path = theScript->path;

		info.internal = static_cast<ScriptMetadata*>(script);
	} else {
		auto thePlugin = static_cast<PluginMetadata*>(script);

		info.name = thePlugin->name;
		info.id = thePlugin->id;
		info.developer = thePlugin->developer;
		info.version = thePlugin->version;
		info.serpentVersion = thePlugin->serpentVersion;
		info.path = thePlugin->path;

		info.native = thePlugin->native;
		info.loaded = thePlugin->loaded;

		info.internal = static_cast<PluginMetadata*>(script);
	}

	info.script = isScript;

	return info;
}