--@name SL Example 03
--@developer YellowCat98
--@version 1.0.0
--@serpent-version 1.0.0
--@id examples.plugintest
--@plugins serpentlua.std yellowcat98.plugintest yellowcat98.nonative_plugintest

-- Basic example for importing other plugins.
-- This isn't really an example made for you to run, just shows how you can include plugins and how to expect them to function.
-- If you are plugin developer, this example is useful in getting to know what script writers expect from your plugin.
-- Assuming `yellowcat98.plugintest` and `yellowcat98.nonative_plugintest` are plugins built for the SerpentLua version this script is using.

local TestPlugin = require("yellowcat98.plugintest")
local NoNativeTestPlugin = require("yellowcat98.nonative_plugintest")
local SL = require("serpentlua.std") -- This plugin unfortunately never catches a break.

TestPlugin.the_Function() -- This is a function exposed by the yellowcat98.plugintest plugin.
SL.log.info(string.format("TestPlugin threw a cool Variable: \"%s\"", TestPlugin.coolVar))

NoNativeTestPlugin.the_Function()
SL.log.info(string.format("NoNativeTestPlugin threw a cool Variable: \"%s\"", NoNativeTestPlugin.coolVar))

SL.log.info("The plugins we rely on™:")
for k, v in pairs(SL.ScriptMetadata.get().plugins) do
    SL.log.info(v)
end