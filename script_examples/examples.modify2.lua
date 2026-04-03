--@name SL Example 05
--@id examples.modify2
--@version 1.0.0
--@serpent-version 1.0.0
--@developer YellowCat98
--@plugins serpentlua.std yellowcat98.modify

-- Basic example for the Modify plugin

local SL = require("serpentlua.std")
local Modify = require("yellowcat98.modify")

Modify.createHook("amazing-hook", "MenuLayer", "onMoreGames", function(self, sender)
	SL.log.info("====MODIFY2 HOOK: BEFORE ORIGINAL====")
	original(self, sender)
	SL.log.info("====MODIFY2 HOOK: AFTER ORIGINAL====")
end)

Modify.createHook("init-hook", "MenuLayer", "init", function(self)
	if not original(self) then return false end
	SL.log.info("YOYO MODIFY2 HOOK")
	return true
end)

Modify.applyHook("amazing-hook")
Modify.applyHook("init-hook")