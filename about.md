# SerpentLua

A rewrite of [Serpent](mod:yellowcat98.serpent) that uses Lua instead of Python and a bunch of cool stuff!

A mod that allows you to create your own Lua scripts and run them within the game.

See the next sections for more information.
## Scripts
A script is a plain Lua file.
SerpentLua sandboxes scripts completely. On their own, scripts shouldn't cause any harm.

## Plugins
A plugin extends what a script can do on its own.
Scripts are useless without plugins, they cannot do anything meaningful.
(without plugins, even stuff like printing to the console is impossible.)

## There are two types of plugins
### Native plugins:
Plugins that are in the form of .slp files, developed specifically for this mod.
They are not sandboxed.
They run with the same privileges that GD is given.

### Non-native Plugins:
Plugins that are created directly using the SerpentLua API.
These plugins are just Geode mods, nothing else.
If they're in the Geode Index (meaning downloaded from the Geode Loader directly), they are likely unsafe.

- if you are interested in making plugins for SL, writing them as a Geode mod (a non-native plugin) is recommended.

## Disclaimer

### Plugin
Using non-approved native plugins is risky.
Native plugins are in the form of DLLs (just with the .slp extension instead), they are not sandboxed. If you're using a non approved native plugin,
**MAKE SURE** you know what you're doing.
If you don't know what a plugin does, immediately delete it.
You can view the next section (after the disclaimer) if you want to verify if a plugin is safe, though most of the time it's better to just delete it.
### Scripts
The only risk with running scripts is the risk of it abusing a plugin which allows it to break out of the sandbox.

## Plugin Verification
### If you have the source code:
Review it.
Recompile it.
If on GitHub, verify the compiled .slp matches the source code.

### If you don't have the source code:
Do not install unless you trust the source.

# By installing this mod, you acknowledge:
- The disclaimer above.
- That YellowCat98 **IS NOT** responsible to any harm that may be caused to your device.
- You are responsible for any plugins or scripts you run.