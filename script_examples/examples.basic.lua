--@name SL Example 01
--@id examples.basic
--@version 1.0.0
--@serpent-version 1.0.0-alpha.2
--@developer YellowCat98
--@plugins serpentlua.std

-- basic script example for serpentlua, logging.

local SL = require("serpentlua.std")

local metadata = SL.ScriptMetadata.get()

SL.log.info("Hello, World!")

SL.log.debug(string.format("Beep Boop! Script %s (ID: %s) is running on version %s <insert more robot noises>", metadata.name, metadata.id, metadata.version))

local stringToPlayAroundWith = "Hello, World."

SL.log.debug(string.format("ANALYSING STRING: %s", stringToPlayAroundWith))
SL.log.debug(string.format("LENGTH: %d", string.len(stringToPlayAroundWith)))
SL.log.debug(string.format("IN UPPERCASE: %s", string.upper(stringToPlayAroundWith)))

SL.log.warn("There's nothing to warn for...? but this could be useful too.")
SL.log.error("There is no error, yet. This function will be useful in the future.")
SL.log.trace("This function is very useful.")

--[[
SL.log: A table containing 4 logging functions.
	info: Typical info log, pretty much.
	debug: Verbose debugging logs. Can only be seen if log level is set to Debug/Trace in Geode settings.
	warn: A warning message, self explanatory.
	error: An error message, self explanatory.
	trace: Highly verbose messages. Can only be seen if log level is set to Trace in Geode settings.
]]