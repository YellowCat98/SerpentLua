--@name SL Example 03
--@id examples.modify
--@version 1.0.0
--@serpent-version 1.0.0-alpha.1
--@developer YellowCat98
--@plugins serpentlua.std yellowcat98.modify

-- Basic example for the Modify plugin

local SL = require("serpentlua.std")
local Modify = require("yellowcat98.modify")

Modify.hook("MenuLayer", "onMoreGames", function(self, sender)
    SL.log.info("BEFORE")
    original(self, sender)
    SL.log.info("AFTER")
end)

Modify.hook("MenuLayer", "init", function(self)
    if not original(self) then return false end
    SL.log.info("HELLO")
    return true
end)

Modify.hook("MenuLayer", "onTwitch", function(self, sender)
    SL.log.info("HELLO")
    SL.log.info("AFTER")
end)