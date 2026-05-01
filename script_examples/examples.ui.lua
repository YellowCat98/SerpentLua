--@name SL Example 05
--@id examples.ui
--@version 1.0.0
--@serpent-version 1.0.0
--@developer YellowCat98
--@plugins serpentlua.std yellowcat98.modify

-- Note! This example uses the Modify plugin by YellowCat98.
-- Get the Modify plugin at: https://github.com/YellowCat98/serpentlua-modify/releases/latest to use this example.

-- Basic example for using the UI system that the SerpentLua plugin provides.

-- Simple counter.

local SL = require("serpentlua.std")
local Modify = require("yellowcat98.modify")

local counter = 0

-- Node is simply a class that wraps around a type that lua doesn't understand (CCNode and its derivatives).
--[[
	Types that Node wraps around:
	CCNode (NodeType.Node)
	CCSprite (NodeType.Sprite)
	CCMenuItemSpriteExtra (NodeType.Button)
	CCLabelBMFont (NodeType.Label)
	CCMenu (NodeType.Menu)
	FLAlertLayer (NodeType.Alert)
]]
-- Any other types that aren't bound are registered of type Node.
-- As long as a type derives from CCNode, it can be registered as Node.
-- With Node, you can call setters and getters (or any method) of any CCNode derivative easily.
-- NodeType is an enum (which is internally just a table of strings corresponding to numbers.)
-- Usage: SL.Enums.ui.NodeType.X

Modify.createHook("hello-hook", "MenuLayer", "init", function(self)
	if not original(self) then return false end
	local selfNode = SL.ui.Node.createFromCCNode(self)
	-- You may use the createFromCCNode function to create a Node instance to be able to call methods of a CCNode.
	-- `self` is a MenuLayer, which derives from CCNode. (i mean it derives from a bunch of stuff but it ultimately goes down to ccnode anyway)
	-- You may also provide an argument containing the type of the node (with NodeType)

	local label = SL.ui.createLabel(string.format("Counter: %d", counter), "bigFont.fnt")
	-- SL.ui.createLabel is just a wrapper around the SL.ui.Node.create(SL.enums.ui.NodeType.Label, {text="...",font="..."})
	-- The same goes for other types.
	label:method("setID")("counter-id")
	-- `method` is a function that returns another function that simply calls the method you want to call.
	-- Usage: node:method("<method-name>")(args...)
	label:method("setPosition")({
		x=SL.ui.getWinSize().width/2,
		y=SL.ui.getWinSize().height/2
	})
	-- getWinSize is self explanatory. Returns a table containing the width and height of the screen.

	local node = SL.ui.createButtonWithSprite("GJ_button_01.png", false, function(sender)
		counter = counter+1
		label:method("setString")(string.format("Counter: %d", counter))
	end)
	-- You may also use SL.ui.createButton to create a button with another Node as its image.
	-- SL.ui.createButtonWithSprite does the same thing as SL.ui.createButton(SL.ui.createSprite(sprite, framename), function).
	-- The first argument is the sprite's name.
	-- The second boolean argument is whether the sprite comes from a sprite sheet or not.
	-- The last argument is the function that gets called when the button is pressed.
	node:method("setID")("my-counter")
	-- Most nodes have an ID that is used to distinguish between nodes.

	local menu = SL.ui.Node.getByID(selfNode, "bottom-menu")
	menu:addChild(node)
	-- Self explanatory, adds a node to a child.
	-- doing addChild on bottom-menu automatically places the button inside it without having to do layout stuff.
	-- You might need to call menu:method("updateLayout")() on some menus if the node isn't placed alongside the other buttons.
	-- As of 1.3.0 SerpentLua does not have any layout support aside from the updateLayout function.

	selfNode:addChild(label)

	-- Below is an example of using the Alert node type.
	local alertButton = SL.ui.createButtonWithSprite("GJ_button_02.png", false, function(sender) -- forgot to mention this, `sender` is just the button itself, though you have to use createFromCCNode to use it within the callback.
		local alert = SL.ui.createAlert("SerpentLua", "Hello from my custom script!", "OK") -- (yes this is a reference to Geode SDK mod example)
		alert:method("show")() -- Alert's only exclusive method is `show`.
	end)

	menu:addChild(alertButton)

	return true
end)

Modify.applyHook("hello-hook")

--[[
	Creating a Node through the manual way.
	SL.ui.createNode <=> SL.ui.Node.create(SL.enums.ui.NodeType.Node, {})
	SL.ui.createSprite <=> SL.ui.Node.create(SL.enums.ui.NodeType.Sprite, {sprite=string, frameName=bool})
	SL.ui.createButton <=> SL.ui.Node.create(SL.enums.ui.NodeType.Button, {image=Node, callback=function})
	SL.ui.createLabel <=> SL.ui.Node.create(SL.enums.ui.NodeType.Label, {text=string, font=string})
	SL.ui.createMenu <=> SL.ui.Node.create(SL.enums.ui.NodeType.Menu, {})
	SL.ui.createAlert <=> SL.ui.Node.create(SL.enums.ui.NodeType.Alert, {title=string, content=string, btn1=string, btn2=string, callback=function})
]]
-- I personally don't see any use case for these functions, but they're here if you want to use them.