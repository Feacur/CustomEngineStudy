# Immediate tasks
- improve physics response, integration, etc.
  - study [box2d-lite](https://github.com/erincatto/box2d-lite)

# Near plans
- extend application of `Config_Asset`
  - (?) put Lua callbacks there (assets, init, update, etc.)
  - (?) file watcher folder path
- allow correct physics processing in presence of `Hierarchy` components

# Bugfix
- investigate nested and inline prefabs instantiation
  - forbid it by ignoring the command and warning the user?
  - still, order of instantiation is error-prone now:
  - fix `promote_to_instance`
- extensively test systems; erroneous behaviour still emerges
- system code performance on my machine drops noticeably if powered from the accumulator only
  - it's probably expected, though  still unwanted, irritating, potentially malicious
- hot reloading Lua might leak junk
- hot reloading prefabs might break something

# Keep in mind
- build times; they seem to be ok right now on my machine (August 31, 2020)
- test shipping configuration from time to time
- test precompiled headers occasionally
- file watcher threads syncronization
  - for the time being I use a simple assertion lock; Naughty Dog does use this technique, though
- sporadic runtime and cleanup crashes are, probably, memory-related
  - revisit your `placement new` calls; btw, you can use templates like
  - ...  for that, but oh is it ugly
- (?) provide or not default components' values
- it's possible to reactively allocate and deallocate components' data;
  - might be useful for the third-party backends
- components, entities, assets can be relocated in memory
- buy [RemedyBG](https://remedybg.itch.io/remedybg)
- search for `@Bug:`, `@Note:`, `@Optimize:`, `@Todo:`, `@Change:` in the code

# Optimization
- smaller data types is not always a win; sometimes junk padding helps performance a substantial amount

# Future features
- `Shared_Library_Asset`
- `Material_Asset`
  - (?) a separete `Physics_Material_Asset`, if ever needed
- assets metadata
- 3d physics, GJK, etc.; [watch Casey's video](https://youtu.be/Qupqu1xe7Io); [read Glenn's article](https://gafferongames.com/post/physics_in_3d/)
- preprocess raw assets so that they can be loaded into memory with little or no parsing
- OS backends: Linux
- graphics backends: DirectX, Vulkan
- physics backends: Box2d, Bullet Physics
- refactor loading interface: hide specific implementation, alike it was done for `*.obj`
  - (?) `loading` for assets and `entity_components` seem a bit confusing
- (?) instanced prefabs should receive update upon hot reloading of prefabs
  - the same applies to promoted ones
- (?) assets usage tracking, so that they could be automatically unloaded?
  - some of them can be quite easily marked like this through components load/unload
  - ... though there are instances, where code requests an asset and should manage the lifetime by itself
- (?) unify renderable mesh with physics mesh
- more types for `Config_Asset`; arrays would be useful, too
- (?) try [CMake](https://cmake.org/)

# Potential features
- (?) unity build option
- (?) reorganize projects path due to inconvenient error messages
  - moving preject files to the root fixes the behaviour, but looks ugly
- move project files out from the root directory
- (?) try symlinks instead of copying files around
  - developer mode or administrator mode required
  - https://blogs.windows.com/windowsdeveloper/2016/12/02/symlinks-windows-10/
  - probably should collapse engine assets and sandbox assets into a single folder for that
  - (???) or run mutiple threaded watchers
- entity system callbacks, signaling appearance of entities with a requested set of components
- (?) embed [dear imgui](https://github.com/ocornut/imgui)
- (?) embed [Box2D](https://github.com/erincatto/box2d)
- (???) explore the idea of [rotors](https://marctenbosch.com/quaternions/) as a quaternions replacement

[Markdown](https://www.markdownguide.org/basic-syntax/)
