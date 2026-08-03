--@name SL Example 01
--@id examples.basic
--@version 1.0.0
--@serpent-version 1.6.0
--@developer YellowCat98
--@plugins serpentlua.std@1.4.0

-- Basic Example for using SerpentLua's standard plugin's most basic features. (Logging, accessing Script/Plugin metadata, string formatting)
local SL = require("serpentlua.std")

local metadata = SL.ScriptMetadata.get()
--[[
	SL.ScriptMetadata: Represents the metadata of a script.
	members: name, id, developer, serpentVersion, version, nostd, plugins
]]
SL.log.info("Hello, World!")

SL.log.debug(SL.fmt.format("Beep Boop! Script {} (ID: {}) is running on version {} <insert more robot noises>", metadata.name, metadata.id, metadata.version))
-- SL.fmt.format uses the same syntax as FMT library for the C++ language.
-- What differentiates it from `string.format` is that you do not need to know the type of the format arguments inside the format string.

local stringToPlayAroundWith = "Hello, World."

SL.log.debug(SL.fmt.format("ANALYSING STRING: {}", stringToPlayAroundWith))
SL.log.debug(SL.fmt.format("LENGTH: {}", string.len(stringToPlayAroundWith)))
SL.log.debug(SL.fmt.format("IN UPPERCASE: {}", string.upper(stringToPlayAroundWith)))
SL.log.debug(SL.fmt.format("IN LOWERCASE: {}", string.lower(stringToPlayAroundWith)))

SL.log.warn("There's nothing to warn for...? but this could be useful too.")
SL.log.error("There is no error, yet. This function will be useful in the future.")
SL.log.trace("Wow! Super Duper Verbose Message.")

--[[
SL.log: A table containing 4 logging functions.
	info: Typical info log, pretty much.
	debug: Verbose debugging logs. Can only be seen if log level is set to Debug or Trace in Geode settings.
	warn: A warning message, self explanatory.
	error: An error message, self explanatory.
	trace: Highly verbose messages. Can only be seen if log level is set to Trace in Geode settings.
]]

local plugin = SL.PluginMetadata.getByID("serpentlua.std")
--[[
	SL.PluginMetadata: represents a plugin.
	keys: name, developer, id, version, serpentVersion
	Note that this is not exclusive to just plugins this script has loaded, but every plugin. Note that plugins that have failed to run do not get recognized.
]]

SL.log.info(SL.fmt.format("The plugin responsible for logging this message is of ID \"{}\".", plugin.id))

-- SL.ScriptMetadata.plugins returns an array of strings, not PluginMetadata object, you must get the PluginMetadata yourself through the id.
for k, v in pairs(metadata.plugins) do
	local p = SL.PluginMetadata.getByID(v)
	-- Do whatever...
end