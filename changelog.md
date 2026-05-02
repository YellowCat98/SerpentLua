# 1.4.0
- Internal: Reorganize code.
- UI: Add a "never show again" option in the `lua.dll` popup.

# 1.3.0
- SL Plugin v1.2.0: Add new Alert node type.

# 1.2.1
- SL Plugin v1.1.1: Basic code improvements.
- UI: Allow multiple scripts/plugins of the same name to appear in the UI.

# 1.2.0
- SL Plugin v1.1.0: Added new `enums` table.
- SL Plugin v1.1.0: Added basic UI utilities.
- Error handling: Show system-level alert on lua panic (used to previously print error and exit immediately)

# 1.1.0
- API: Expose `ScriptMetadata::getScriptByState`.
- API: Expose `ScriptMetadata::getScriptByID`.
- UI: Alphabetically sort scripts and plugins by name. (serpentlua std is always on top btw)

# 1.0.2
- Internal: Compare versions while taking Semantic Versioning into account.
- Script/Plugin metadata: Allow leading `v` in version.
- Script/Plugin metadata: Error check version.
- UI: Always have a leading `v` in version regardless of whether the version has one or not.

# 1.0.1
- Internal: Properly handle error for `createDirectory` functions.

# 1.0.0
- Internal: use Geode's `pathToString` utility and remove useless `.string()` calls.

# 1.0.0-alpha.2
- UI: Make missing lua.dll popup better.
- UI: Plugins that were never used show up in Plugins list.
- UI: Add plugin indicator.
- UI: Add delete script/plugin buttons.
- UI: Add restart required indicator.
- API: Add `native` key to `PluginMetadata`.
- API: Add `path` key to `PluginMetadata`.
- Plugin: Separate `serpentlua.std` plugin version from SerpentLua version.

# 1.0.0-alpha.1
- Initial release