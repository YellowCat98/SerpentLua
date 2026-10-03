# SerpentLua

A rewrite of [Serpent](mod:yellowcat98.serpent) that uses Lua instead of Python and a bunch of cool stuff!

A mod that allows you to create your own Lua scripts and run them within the game.

Using LuaJIT 2.1.

See the next sections for more information.

## Scripts
- A script is a plain Lua file.
- SerpentLua sandboxes scripts completely. On their own, scripts shouldn't cause any harm.

## Plugins
- A plugin extends what a script can do on its own.
- Scripts cannot do anything meaningful without plugins. This includes even stuff as simple as printing to the console.
- A plugin is essentially a Geode mod, it uses interfaces/API that SerpentLua exposes to mod developers to be able to extend what SerpentLua scripts can do.

## Disclaimer
- Although scripts are sandboxed, you should still not run any scripts unless you know exactly what they do.
- Do not install plugins that are not on the Geode Index. Geode itself already warns you of this.

## Documentation
- Check out the README at the [GitHub Repository](https://github.com/yellowcat98/SerpentLua) for setting up SerpentLua and documentation.

## Possible questions:
- **Q: Why is the plugins list not showing a plugin that I have installed?**  
  A: SerpentLua doesn't show plugins that have failed to load. Check for errors in the platform console.

- **Q: Why do scripts say a plugin doesn't exist when it does?**  
  A: Scripts cannot recognize plugins that have failed to load. Check for errors in the platform console.

- **Q: What is the platform console?**  
  A: The platform console is an additional window that opens alongside GD that shows logs. It is recommended to have it on as SerpentLua logs errors.


## Notes:
- Enabling the platform console in the Geode settings is encouraged for better error handling.
- The SerpentLua plugin uses a separate version from SerpentLua itself.

## Repositories of interest:
- [SerpentLua Plugin Index](https://github.com/yellowcat98/serpentlua-server): The source code of the SerpentLua server.

## Community
- The [SerpentLua Discord server](https://discord.gg/qnPgmUVZsV) is where you can share your own scripts or ask any questions you might have.

## By installing this mod, you acknowledge:
- That YellowCat98 **IS NOT** responsible to any harm that may be caused to your device.
- You are responsible for any plugins or scripts you run.