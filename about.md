# SerpentLua

A rewrite of [Serpent](mod:yellowcat98.serpent), but instead of python, it's lua, and a few more cool stuff.

A mod that allows you to create your own lua scripts and run them within the game.

See the next sections for more information.
## Scripts
A script is a plain lua file.
SerpentLua sandboxes scripts completely. On their own, they cannot cause any harm.

## Plugins
A plugin extends what a script can do on its own.
Scripts are useless without plugins, they cannot do anything meaningful.
(without plugins, there is no printing hello world to the console.)

## There are two types of plugins
### Native plugins:
Plugins that are in the form of .slp files, developed specifically for this mod.
They are not sandboxed.
They run with the same privileges that GD is given.

### Non-native Plugins:
Plugins that are created directly through code, typically by other Geode mods, allowing for Geode developers to create their own lua bindings.
These are not DLLs, they are initialized and managed by the Geode mod that created it.
Since these Plugins are just Geode mods, they do not require approval, they are reviewed by the Geode Index Staff team like every other mod, these plugins are considered safe.

- if you are interested in making plugins for SL, writing them as a Geode mod (a non-native plugin) is recommended.

## DISCLAIMER

### PLUGINS
Using non-approved native plugins is risky.
Native plugins are in the form of DLLs (just with the .slp extension instead), they are not sandboxed. If you're using a non approved native plugin,
**MAKE SURE** you know what you're doing.
If you don't know what a plugin does, immediately delete it.
You can view the next section (after the disclaimer) if you want to verify if a plugin is safe, though most of the time it's better to just delete it.
### SCRIPTS
The only risk with running scripts is the risk of it abusing a plugin which allows it to break out of the sandbox.

## PLUGIN VERIFICATION
### IF YOU HAVE THE SOURCE CODE
Review it.
Recompile it.
If on GitHub, verify the compiled .slp matches the source code.

### IF YOU DON'T HAVE THE SOURCE CODE
Do not install unless you trust the source.

# BY INSTALLING THIS MOD, YOU ACKNOWLEDGE:
- The disclaimer above.
- That YellowCat98 **IS NOT** responsible to any harm caused to your device.
- You are responsible for any plugins or scripts you run.