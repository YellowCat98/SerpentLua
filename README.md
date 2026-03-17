# SerpentLua

- A Geode mod for coding scripts in the Lua language.

- View the about.md file within this repository for more information.

# Setup:
- Plugins use a global dependency `lua.dll`. Plugins will not load if this file does not exist.
1. Open Geometry Dash and press the SerpentLua button.
![Visual Step 1](README_resources/sl_btn_pointer.png)
2. Press the settings button at the bottom.
![Visual Step 2](README_resources/settings_btn_pointer.png)
3. Press the config button at the bottom.
![Visual Step 3](README_resources/conf_btn_pointer.png)
4. Enter the `plugin_global_deps` folder.
![Visual Step 4](README_resources/plugin_dir_pointer.png)

# Scripts:
## Creating your first script:
- Simply create a .lua file (preferably in the `author.script_name` format.)
- At the first few lines, that is where we insert our metadata.
- In order for them to not interfere with the Lua interpreter, they are in the form of comments.
- Example metadata:
> ```lua
> --@name ScriptName
> --@developer Author
> --@id author.script_name
> --@version 1.0.0
> --@serpent-version 1.0.0-alpha.1
> --@plugins yellowcat98.plugintest serpentlua.std
> ```
- name: The name of the script.
- developer: The developer of the script.
- id: internal ID SerpentLua will use to identify this script. (note!!! Scripts will not load if their .lua file doesn't correspond with their ID!)
- version: The version of the script.
- serpent-version: The version of SerpentLua that the script targets.
- plugins: The plugins the script relies on. Separated by spaces.
- nostd (Optional!): Whether to import the Lua standard library or not. This option defaults to true when not provided. (this option really only exists for the sake of existing, really.)

- The first 7 lines must be Metadata. SerpentLua rejects any script where the 1st-7th lines aren't metadata.

## Script examples worth looking at:
- [Basic Example](script_examples/examples.basic.lua): Basic example for the standard SerpentLua plugin. This script showcases logging capabilities.

- [Playground Example](script_examples/examples.playground.lua): Basic example for the playground feature in the standard SerpentLua plugin. A library that gives sandboxes access to a directory for each plugin.

- [Modify example](script_examples/examples.modify.lua): Basic example for the Modify plugin. A plugin that allows modifying Geometry Dash. This plugin is yet to be finished.

- [Plugin Test Example](script_examples/examples.plugintest.lua): Basic example for the Plugin examples in plugin_examples/yellowcat98.plugintest.

# Plugins:
- Note: this is rather advanced. If you're looking just to create scripts, skip this.
## Creating your first native plugin:
- This will only include how plugins are written. View [this](plugin_examples/yellowcat98.plugintest/README.md) for compilation.
0. Get your `lua.lib` and specifically lua version `5.4.6`. (This step was added last and i just wasnt feeling like making it step 1 and incrementing the other steps lol)
1. Create a `plugin.slm` file inside the root of the project. Inside it, write your plugin metadata. Plugin metadata is written the exact same as Script metadata with the exception of removing the `--@nostd` and `--@plugins` keys.
2. Create a `serpentlua.rc` file containing the following:
`SERPENTLUA_METADATA RCDATA "plugin.slm"`
3. Create your `main.cpp` source file.
4. Include the Lua C API.
5. Declare the struct `__metadata`:
> ```c++
> struct __metadata {
>     const char* name;
>     const char* developer;
>     const char* id;
>     const char* version;
>     const char* serpentVersion;
> };
> ```
4. Declare the struct `SerpentLuaAPI`:
> ```cpp
> struct SerpentLuaAPI {
>     void (*log)(__metadata, const char*, const char*); // Basic logging function for plugins.
>     __metadata metadata; // Allows access to your plugin's metadata.
>     HMODULE handle; // Your Plugin's HMODULE.
> };
> ```
5. Declare the a static variable of type `SerpentLuaAPI` (must be of any name, example: `static SerpentLuaAPI api;`)
6. Declare `initNativeAPI`
> ```c++
> // Important! must be exported with `extern "C"` as SerpentLua will call it!
> extern "C" __declspec(dllexport) void initNativeAPI(SerpentLuaAPI TheCoolAPI) {
>     api = TheCoolAPI; // `api` being the static variable you declared the previous step.
> }
6. Declare your `entry` function with the signature `extern "C" __declspec(dllexport) void entry(lua_State* L);`. lua_State* L being the lua interpreter of whatever script is using your plugin.
7. Inside your entry function, create a table, this table is where you store everything about your plugin.
8. Make sure to insert the table inside the `serpentlua_modules` table in order to expose it to scripts.
9. Compile it using [this](plugin_examples/yellowcat98.plugintest/README.md) example.

## Native plugin example worth looking at:
[PluginTest](plugin_examples/yellowcat98.plugintest): A basic plugin example. Exposes a `the_Function` and a `coolVar` variable to lua.

## Creating your first non-native plugin as a Geode mod:
- Note: Prior Geode SDK knowledge is highly recommended.
1. Add `yellowcat98.serpentlua` as a dependency in your `mod.json`
2. Inside your `$on_mod(Loaded)` or `$execute`...
- To be finished.