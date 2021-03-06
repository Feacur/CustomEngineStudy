# September 19, 2020
- extended `Config_Asset`: Lua callbacks there (assets, init, update, etc.)
- extended `Config_Asset`: file watcher folder path
- made strings and identifiers more strict in the assets
- parsing wrappers don't consume previous identifiers, but void part only
- alert in debug mode about potentially erroneous assets
- warn about missing configuration options

# September 13, 2020
- implement crude friction response
- implement crude angualar motion physics
- store collider meshes counterclockwise
- expose new physics settings into the config file
- improve VSCode errors reporting
- fix potential compilation failure
- handle linear math types as const references
- correctly close file handles
- simplify build target names

# September 06, 2020
- fixed buf with memory relocations during prefab assets loading
- basic semi-implicit euler integration, physics response
- made rendering and physics systems interface more explicit about the resources
- implemented full reset of the state, so that `main.lua` could be reloaded correctly

# September 05, 2020
- factor physics mesh asset out of the prefab data
- factor physics backend data out of the component
- collision SAT normal is correct now for any orientation
- don't `placement new` components; also don't clear them, but prepare upon loading
- correctly convert between 3d and 2d rotations

# September 04, 2020
- hot reloading of assets
  - shaders, textures, meshes, prefabs, configs, lua assets
- gave a shot to symlinks: doable, nice in a way
  - ugly in others: hot reloading paths are mess; need a revamp of assets structure

# September 03, 2020
- make use of `Config_Asset`
  - put graphics settings there (both context and runtime)

# September 02, 2020
- interactive physics demo (simple SAT)
  - `Phys2d` component
- provided some more runtime checks for components and assets
- initial unused `Config_Asset`

# September 01, 2020
- provided this [DONE](DONE.md) along with [TODO](TODO.md)
- fixed ref generations tracking
- implemented serializable components removal
  - override removes missing components, too
- `*.obj` tris count per face is unlimited now
- partially fixed automatic instantiation

# August 31, 2020
- fixed strings storage comparisons
- finilized [and fixed] file watcher
  - might have thread synchronization issues
- refactored prefabs
- provided initial physics test boilerplate

[Markdown](https://www.markdownguide.org/basic-syntax/)
