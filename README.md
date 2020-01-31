# Summary
Study on C/C++, custom engines, games and stuff  

# How to work with this stuff
### __See sub-projects inside folder__
```
> __ workspace_v1 __
VSCode workspace, because it doesn't support nested projects
```

```
> code/demo_game
Game layer, coupled with platform_windows layer

pathtracing ..... reading Peter Sherley's book
tetris .......... basic mechanics
conway_life ..... basic mechanics
pacman .......... WIP basic mechanics
sound_test ...... toying with DSound
study_raymarch .. basic raymarching and SDF
study_raytrace .. basic raytracing
entity_system ... toying with ECS
```

```
> code/platform_windows
Windows platform layer, coupled with demo_game layer
```

```
> code/demo_console
Standalone console application test bed, C++

list ............ testing intrusive linked list
```

```
> code/demo_c
Standalone console application test bed, C

test ............ testing C code
```

### __See sub-projects inside folder__
```
> __ workspace_v2 __
VSCode workspace, because it doesn't support nested projects
```

```
> engine
Engine project
```

```
> sandbox
Executable test project
```

### __Project setup and build__
Initial  
* git submodule update --init --recursive
* git submodule update -f

In case of using SSH key passphrases  
* See "/__ external settings __/.gitconfig"

VSCode  
* See "**/.vscode/tasks.json" for build commands
* See "**/.vscode/launch.json" for run and debug commands

Premake  
* Call GenerateProjects_v1.bat
* Call GenerateProjects_v2.bat
* Use your IDE of preference

> Target output into "/bin" folder  
> Intermediate output into "/bin-int" folder  

# Current state
* app 1: ".exe" platform + hot reloadable ".dll" game module
* app 2: ".exe" only console, written in C++
* app 3: ".exe" only console, written in C
* abstract data structures and algorithms

# To do
* read the pathtracing book, implement
* finish pacman
* data with handles instead of pointers
* memory management
* assets management
* "Entity Component System" framework
* physics, 2d for starters
