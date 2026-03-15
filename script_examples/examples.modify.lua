--@name SL Example 03
--@id examples.modify
--@version 1.0.0
--@serpent-version 1.0.0
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