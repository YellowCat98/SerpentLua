# SerpentLua

A rewrite of [Serpent](mod:yellowcat98.serpent) that uses Lua instead of Python and a bunch of cool stuff!

A mod that allows you to create your own Lua scripts and run them within the game.

Uses Lua Version: 5.4

See the next sections for more information.

## Scripts
- A script is a plain Lua file.
- SerpentLua sandboxes scripts completely. On their own, scripts shouldn't cause any harm.

## Plugins
- A plugin extends what a script can do on its own.
- Scripts cannot do anything meaningful without plugins. This includes even stuff as simple as printing to the console.

## There are two types of plugins
### Native plugins:
- Plugins that are in the form of .slp files, developed specifically for this mod.
- They are not sandboxed.
- They run with the same privileges that GD is given.

### Non-native Plugins:
- Plugins that are created directly using the SerpentLua API.
- These plugins are just Geode mods, nothing else.
- If they're in the Geode Index (meaning downloaded from the Geode Loader directly), they are likely safe.
- Non-native plugins are meant for Geode developers who wish to integrate their mods within SerpentLua.
- Creating plugins as non-native plugins is generally recommended more than creating them as native plugins.

## Disclaimer!!

### Plugins
- Using non-approved native plugins is risky.
- Native plugins are in the form of DLLs (just with the .slp extension instead), they are not sandboxed.
- If you're using a non-approved native plugin, **MAKE SURE** you know what you're doing.
- If you aren't sure what a plugin, immediately delete it.
- You can view the next section (after the disclaimer) if you want to verify if a plugin is safe, though most of the time it's better to just delete it.
### Scripts
- The only risk with running scripts is the risk of it abusing a plugin that allows it to break out of the sandbox.

## Plugin Verification
### If you have the source code:
- Review it.
- Recompile it.
- If on GitHub, verify the .slp matches the source code.

### If you don't have the source code:
Do not install unless you trust the source.

## Documentation
- Check out the README at the [GitHub Repository](https://github.com/yellowcat98/SerpentLua) for setting up SerpentLua and documentation.

## Possible questions:
- **Q: Why is the plugins list not showing a plugin that I have installed?**  
  A: SerpentLua doesn't show plugins that have failed to load. Check for errors in the platform console.

- **Q: Why do scripts say a plugin doesn't exist when it does?**  
  A: Scripts cannot recognize plugins that have failed to load. Check for errors in the platform console.

- **Q: What is the platform console?**  
  A: The platform console is an additional window that opens alongside GD that shows logs. It is recommended to have it on as SerpentLua logs errors.

- **Q: What is Error 126?**  
  A: Error 126 corresponds to the "module not found" error in Windows. Please make sure you have `lua.dll` in `plugin_global_deps`. (If so, try reinstalling it from whatever release of SerpentLua you have.)

## Notes:
- Enabling the platform console in the Geode settings is encouraged for better error handling.

## Plugins of interest:
- [YellowCat98 Modify plugin](https://github.com/yellowcat98/serpentlua-modify): Modify GD functions to extend what a script can do.

## Discord
- If you have any questions or would like to share a script/plugin you created, join the [Discord](https://discord.gg/qnPgmUVZsV)!

## By installing this mod, you acknowledge:
- The disclaimer above.
- That YellowCat98 **IS NOT** responsible to any harm that may be caused to your device.
- You are responsible for any plugins or scripts you run.