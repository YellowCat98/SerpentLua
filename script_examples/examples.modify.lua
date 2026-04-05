--@name SL Example 03
--@id examples.modify
--@version 1.0.0
--@serpent-version 1.0.0
--@developer YellowCat98
--@plugins serpentlua.std yellowcat98.modify

-- Basic example for the Modify plugin (Icon Hack)

--[[
	Terms used:
		Detour: The function that will get called instead of the original.
		Hook: Replace the function call of a specific function with another.
		Self: The class which the function that is being hooked corresponds to.
		Original: The original function that is being hooked.
		Static functions: Functions that do not require a `self` parameter.
]]

local SL = require("serpentlua.std")
local Modify = require("yellowcat98.modify")

--[[
	createHook: Registers a hook.
		ID: Identification for the hook.
		cls: Class you want to hook.
		fn: Function in class you want to hook.
		detour: The function that gets called instead of the original.
]]
Modify.createHook("icon-hack", "GameManager", "isIconUnlocked", function(self, itemType, id)
	-- You may call original through the `original` function defined only within the detour's scope.
	-- `self` must be passed to every detour with the exception of static functions.
	-- `type` and `id` are simply arguments that isIconUnlocked expects.
	return true
	-- This hook simply returns `true` for whenever isIconUnlocked is called.
end)
-- What this hook does:
--[[
	This function is called when checking for if a specific icon is unlocked. (obviously lol)
	This fools the game into thinking that you have every icon unlocked.
]]

-- Another example (this enables practice music sync.)
-- itemType is an enum. Lua automatically converts enums to numbers.
-- 0xC corresponds to UnlockType::GJItem.
Modify.createHook("practice-music-sync", "GameStatsManager", "isItemUnlocked", function(self, itemType, id)
	if itemType == 0xC and id == 17 then
		return true
	else
		return original(self, itemType, id)
	end
end)

--[[
	applyHook:
		Self explanatory. Applies the hook. This is best done early.
]]
Modify.applyHook("icon-hack")
Modify.applyHook("practice-music-sync")