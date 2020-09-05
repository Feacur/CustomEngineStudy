# Immediate tasks
- better physics overall
- factor physics backend data out of the component
- factor physics mesh asset out of the prefab data
  - (?) unify renderable mesh with physics mesh

# Near plans
- extend application of `Config_Asset`
  - (?) put Lua callbacks there (assets, init, update, etc.)
  - (?) file watcher folder path

# Bugfix
- investigate nested and inline prefabs instantiation
  - forbid it by ignoring the command and warning the user?
  - still, order of instantiation is error-prone now:
  - fix `promote_to_instance`
- extensively test systems; erroneous behaviour still emerges
- system code performance on my machine drops noticeably if powered from the accumulator only
  - it's probably expected, though  still unwanted, irritating, potentially malicious

# Keep in mind
- build times; they seem to be ok right now on my machine (August 31, 2020)
- hot reloading Lua might leak junk
- hot reloading prefabs might break something
- smaller data types is not always a win; sometimes junk padding helps performance a substantial amount
- test shipping configuration from time to time
- test precompiled headers occasionally
- file watcher threads syncronization
  - for the time being I use a simple assertion lock; Naughty Dog does use this technique, though
- do I need an inline [DONE](DONE.md)? there are commit messages all the same
  - though, might be used to show big picture
- sporadic runtime and cleanup crashes are, probably, memory-related
  - revisit your `placement new` calls; btw, you can use templates like
  - ...  for that, but oh is it ugly
- (?) provide or not default components' values
- search for `@Bug:`, `@Note:`, `@Optimize:`, `@Todo:`, `@Change:` in the code

# Optimization
- (?) smaller data types for ids

# Future features
- `Shared_Library_Asset`
- `Material_Asset`
  - (?) a separete `Physics_Material_Asset`, if ever needed
- assets metadata
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

[Markdown](https://www.markdownguide.org/basic-syntax/)
