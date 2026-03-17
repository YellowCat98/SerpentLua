--@name Example 04
--@developer YellowCat98
--@version 1.0.0
--@serpent-version 1.0.0-alpha.1
--@id examples.plugintest
--@plugins serpentlua.std yellowcat98.plugintest yellowcat98.nonative_plugintest

-- Basic example for the yellowcat98.plugintest.

local TestPlugin = require("yellowcat98.plugintest")
local NoNativeTestPlugin = require("yellowcat98.nonative_plugintest")
local SL = require("serpentlua.std") -- This plugin unfortunately never catches a break.

TestPlugin.the_Function() -- This is a function exposed by the yellowcat98.plugintest plugin.
SL.log.info(string.format("TestPlugin threw a cool Variable: \"%s\"", TestPlugin.coolVar))

NoNativeTestPlugin.the_Function()
SL.log.info(string.format("NoNativeTestPlugin threw a cool Variable: \"%s\"", NoNativeTestPlugin.coolVar))