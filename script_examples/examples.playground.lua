--@name SL Example 02
--@id examples.playground
--@version 1.0.0
--@serpent-version 1.0.0-alpha.1
--@developer YellowCat98
--@plugins serpentlua.std

--Script example for using the Playground.

local SL = require("serpentlua.std")
SL.playground:init()
local pg = SL.playground

--==== BASIC USAGE ====--

local file = pg.File.create("script://HelloWorld.txt", false)
--[[
File: Represents a file in the playground. Will be created if it doesn't exist.
	Args:
		path: (string)
			Where to create the file.
			using script:// points to a directory that is shared across all users on Windows.
			using user:// points to a directory that is only accessible to the current logged in user on Windows.

		readOnly: (bool)
			Determines if the file can be written to.
]]
SL.log.debug(string.format("File of path %s has been created with name: %s", file:getPath(), file:getName(true)))
--[[
	getPath: (string)
		Returns the absolute path of the file.
	getName: (string)
		Returns the filename of the file.
		Args:
			withExtension: (bool)
				Whether to include the extension or not.
				If not provided, this variable will default to true.
]]

--==== ERROR HANDLING ====--

function hasErrs(myFile)
	for i = 1, myFile:errSize() do
		SL.log.error(string.format("%s: %s", myFile:getName(true), myFile:getErr(i)))
	end
	return myFile:hasErrs()
end

if hasErrs(file) then return end

-- Errors are stored in an array.
--[[
	hasErrs: (bool)
		Determines if the file returned any errors
	errSize: (number)
		Returns how many errors are there.
		Uses 1-based indexing to match Lua.
	getErr: (string)
		Returns an error based on argument.
		Note! This uses 1-based indexing to match Lua. (meaning, the first item is at 1)
		Args:
			index (number):
				Where the error is located
]]

--==== FILE OPERATIONS ====--

local contents = file:read()
--[[
	read: (string)
		Returns the contexts of a file.
]]
SL.log.info(string.format("Listing file contents: \n%s", contents))

local whatToAppend

if contents == "" then
	whatToAppend = "Hello World" --Ensure that on first time running, the starting character is not \n
else
	whatToAppend = "\nHello World"
end

file:append(whatToAppend)
--[[
	append:
		Adds data to file, doesn't overwrite any existing content.
		Args:
			what (string):
				What to append to file.
]]

local newFile = pg.File.create("script://TestFolder/HelloThe.txt", false)
--Folders are created automatically if they do not exist.

if hasErrs(newFile) then return end

newFile:write("NEW DATA!")
--[[
	write: Overwrites all data with the given argument
	Args:
		what: (string)
			What to write to file.
]]

if newFile:erase() == 0 then
	SL.log.info("File erased successfully.")
end
--[[
	erase: (number)
		Deletes a file. Returns 0 if successful, returns 1 if it wasn't successful.
]]

--==== ERROR SCENARIOS ====--

--Certain error scenarios:
local function check(file)
	if hasErrs(file) then
		SL.log.info("Operation failed.")
	end
end

check(pg.File.create("script://../superBadFile.txt", false))
--Playground doesn't allow `..`, `.`, and symlinks.

check(pg.File.create("The The", false))
--Invalid playground syntax.

check(pg.File.create("TheThe://The.The", false))
--Unknown scheme `TheThe`

local notReallyAnError = pg.File.create("script://meowmeow.txt", true)
--Not an error, though it's read-only.

if hasErrs(notReallyAnError) then return end

notReallyAnError:append("Hello")
notReallyAnError:write("NO")
--Both of these lines silently fail because notReallyAnError is read-only.

--==== FOLDER OPERATIONS ====--

local folder = pg.Folder.create("script://HelloFolder") --Represents a folder on the system
--The folder is created if it doesn't exist already.
--[[
	Folder is similar to File with the following functions existing in both tables:
	All of the error handling functions (hasErrs, getErr, errSize) and the erase function.
	(These functions function the exact same as in File.)
]]
if hasErrs(folder) then return end


if folder:exists("CoolFile.txt") then
	SL.log.info("It exists!")
end
--[[
	exists: (bool)
		Returns whether the item provided exists or not.
		Args:
			item: The file to check for.
]]

local coolerFile = pg.File.create("script://FolderTest/CoolFile.txt", false) --Adding a file to a folder is as simple as doing this.

for _, item in pairs(folder:items()) do
	SL.log.info(string.format("Type: %s\nName: %s", item.type, item.item))
end
--[[
	items: (array of tables)
		Returns a table containing an array of tables with each table containing two items:
		type: (string)
			Can only be "file" or "folder". Determines what type it is.
		item: (string)
			Returns the path of the item in the scheme://file format.
]]