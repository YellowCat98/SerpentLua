# SerpentLua

A rewrite of [Serpent](mod:yellowcat98.serpent), but instead of python, it's lua, and a few more cool stuff.

A mod that allows you to create your own lua scripts and run them within the game.

See the next sections for more information.
## Scripts
Self explanatory, a script that runs lua code.

## Plugins
A plugin is basically just a thing that lets your scripts have more stuff, rather than being limited to what SerpentLua provides, C/C++ developers can create their own plugins.

## There are two types of plugins
### Native plugins:
Plugins that are in the form of DLLS, developed specifically for this mod,
Plugins like these require approval before you can just use them for your scripts.

### Non-native Plugins:
Plugins that are created directly through code, typically by other Geode mods, allowing for Geode developers to create their own bindings.
These are not DLLs, they are initialized and managed by the Geode mod that created it.
Since these Plugins are just Geode mods, they do not require approval, they are reviewed by the Geode Index Staff team like every other mod, these plugins are considered safe.


## DISCLAIMER

### PLUGINS
Using non-approved native plugins is a slippery slope.
Native plugins are in the form of DLLs, and they are not sandboxed, if you're using a non approved native plugin,
**MAKE SURE** you know what you're doing.
If you don't know what a plugin does, immediately delete it.
You can view the next section (after the disclaimer) if you want to verify if a plugin is safe, though most of the time it's better to just drop the DLL in the recycle bin.
### SCRIPTS
Unlike plugins, scripts are just lua files, you can read how its written exactly just by opening it in a text editor, read it before running it.
If you don't know how to read lua code, you can hand over the file for someone who knows lua to read, or just delete the lua file directly.

## PLUGIN VERIFICATION
### IF YOU HAVE THE SOURCE CODE
If you have the source code AND the DLL, this could either be legit, or not.
A bad actor could give you the source code and the DLL itself, but lie about the source code.
If you don't understand C/C++, just immediately delete the DLL and the source, or hand over the DLL and source to someone else who knows what they're doing.
If you know C/C++, read the source code line by line, and recompile it all, instead of using the binary provided. the DLL could be the real bad one, while the source code is just a cover to appear good.
(also forgot to mention, plugins can be written in C too, which is pretty cool but i dont see why would you really do that)

### IF YOU DON'T HAVE THE SOURCE CODE
For most users, just delete the DLL, you don't know what the DLL does once it's injected.
But if you know how to reverse engineer, then go for it.

# BY INSTALLING THIS MOD, YOU ACKNOWLEDGE:
- The disclaimer above.
- That YellowCat98 **IS NOT** responsible to any harm caused to your device.
- You are responsible for any plugins or scripts you run.