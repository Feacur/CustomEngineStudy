# Immediate tasks
- interactive physics demo

# Near plans
- hot reloading of assets
  - link file watcher's output with the asset system
- `Config_Asset`
  - put graphics settings there (both context and runtime)
  - (?) put Lua callbacks there (assets, init, update, etc.)
  - (?) file watcher folder path

# Bugfix
- investigate nested and inline prefabs instantiation
  - forbid it by ignoring the command and warning the user?
  - still, order of instantiation is error-prone now:
  - fix `promote_to_instance`
- hot reloading Lua might leak junk
- extensively test systems; erroneous behaviour still emerges
- system code performance on my machine drops noticeably if powered from the accumulator only
  - probably, it's probably expected, but still unwanted

# Keep in mind
- build times; they seem to be ok right now on my machine (August 31, 2020)
- test shipping configuration from time to time
- test precompiled headers occasionally
- file watcher threads syncronization
  - for the time being I use a simple assertion lock; Naughty Dog does use this technique, though
- do I need an inline [DONE](DONE.md)? there are commit messages all the same
  - though, might be used to show big picture
- search for `@Bug:`, `@Note:`, `@Optimize:`, `@Todo:`, `@Change:` in the code

# Optimization
- (?) smaller data types for ids

# Future features
- `Shared_Library_Asset`
- `Material_Asset`
  - (?) a separete `Physics_Material_Asset`, if ever needed
- assets metadata
- OS backends: Linux
- graphics backends: DirectX, Vulkan

# Potential features
- (?) unity build option
- (?) reorganize projects path due to inconvenient error messages
  - moving preject files to the root fixes the behaviour, but looks ugly
- move project files out from the root directory

[Markdown](https://www.markdownguide.org/basic-syntax/)
