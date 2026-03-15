# Test plugin

- a test plugin!

# Compiling
- use any compiler you want as long as it's compatible with the clang compiler.

1. gather lua lib and lua headers
2. set the lua headers
3. link lua lib (dynamically link)
4. create the resource file using the provided `serpentlua.rc` (you can use `llvm-rc`)
5. link the generated .res file
6. compile!
7. place the binary in serpentlua's config directory in the plugins directory
-# (the config directory can be found by `gdroot/geode/config/yellowcat98.serpentlua`)