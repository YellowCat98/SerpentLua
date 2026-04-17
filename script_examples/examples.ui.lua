--@name SL Example 05
--@id examples.ui
--@version 1.0.0
--@serpent-version 1.0.0
--@developer YellowCat98
--@plugins serpentlua.std yellowcat98.modify

-- Note! This example uses the Modify plugin by YellowCat98.
-- Get the Modify plugin at: https://github.com/YellowCat98/serpentlua-modify/releases/latest to use this example.

local SL = require("serpentlua.std")
local Modify = require("yellowcat98.modify")

Modify.createHook("hello-hook", "MenuLayer", "init", function(self)
	if not original(self) then return false end
	local selfNode = SL.ui.Node.createFromCCNode(self)

	local node = SL.ui.createButtonWithSprite("GJ_arrow_01_001.png", true, function(sender)
		SL.log.info("Hello World")
	end)
	node:method("setID")("yo-yo")

	local menu = SL.ui.Node.getByID(selfNode, "bottom-menu")
	menu:addChild(node)

	return true
end)

Modify.applyHook("hello-hook")