# TODO

- (the top most unchecked todos are for the next SerpentLua update, the ones at the very bottom (marked Unrecognized) are unplanned for next update but still in mind. It mostly has breaking changes and stuff i dont want to deal with now)

## 1.4.0
- Plugins: Plugin repository.

## 1.3.0
- ~~SL Plugin (1.2.0): Add FLAlertLayer utilities.~~ ✓

## 1.2.0
- ~~SL Plugin (1.1.0): Basic UI utilities.~~ ✓
- ~~UI: Add ID to SerpentLua button.~~ ✓

## 1.1.0
- ~~ScriptMetadata (1.1.0): Add a `getFromState` function to ScriptMetadata.~~ ✓
- ~~UI (1.0.0): Alphabetically list scripts/plugins by name.~~ ✓

## 1.0.2
- ~~version stuff idk (1.0.2): Follow semver.~~ ✓

## 1.0.0
- ~~Native Plugin API (1.0.0): Access script metadata through ID and state~~ ✓
- ~~RuntimeManager (1.0.0): Add RuntimeManager::getFromState to retrieve metadata through lua_State* alone.~~ ✓

## 1.0.0-alpha.2
- ~~SerpentLua Standard (1.0.0-alpha.2): Make plugin metadata accessible for scripts.~~ ✓
- ~~SerpentLua Geode API: Add a createFromMod(Mod* mod) function to PluginMetadata.~~ ✓
- ~~SerpentLua Standard: Set Version in metadata with NonVString.~~ ✓
- ~~SerpentLua Script/Plugin Loading: Make the mod wait until every plugin has finished loading.~~ ✓
- ~~README: Finish the README~~ ✓
- ~~README: Document dynamic plugin dependencies.~~ ✓
- ~~SerpentLua: Remove useless includes in `main.cpp`~~ ✓
- ~~Scripts UI (1.0.0-alpha.2): Delete script button~~ ✓
- ~~Plugins UI (1.0.0-alpha.2): Visual Indicator between native plugins and non-native plugins.~~ ✓
- ~~Plugins UI (1.0.0-alpha.2): Keep Unloaded plugins' PluginMetadata in order to display them in Plugins UI.~~
- ~~UI (1.0.0-alpha.2): Make missing lua.dll popup #Better.~~ ✓
- ~~UI (1.0.0-alpha.2): Add an indicator when changes have been made~~ ✓

## Uncategorized
- Native Plugin API (v2): Don't require metadata as first argument for the logging functions.

## 1.5.0 (After 1.4.0)
- UI: Fix padding.
- Settings: Add a "install lua.dll" option and make the lua.dll popup redirect you there.
- SL Plugin (1.3.0): Add a `Notification` type that wraps around `