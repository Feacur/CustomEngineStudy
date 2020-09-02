# Summary
Study on C/C++, custom engines, games and stuff  
* See [TODO](TODO.md) and [DONE](DONE.md) lists

# How to work with this stuff
### __Contents__
```
> __ workspace __
VSCode workspace
```

```
> custom_engine
Engine project
```

```
> sandbox
Executable test project
```

### __Project setup and build__
Git  
* `"git submodule update --init --recursive"`
* `"git submodule update -f"`

In case of using SSH key passphrases  
* See `git config example` [there](./__%20external%20settings%20__/.gitconfig)
* See `ssh config example` [there](./__%20external%20settings%20__/ssh%20config)

VSCode  
* See `build commands` [there](./__%20workspace%20__/.vscode/tasks.json)
* See `run and debug commands` [there](./sandbox/.vscode/launch.json)

Other IDEs  
* Call `"GenerateProjects.bat ###"`

> Target output goes into `"/bin"` folder  
> Intermediate output goes into `"/bin-int"` folder  

### __External__
```
> custom_engine/vendor/glad
OpenGL functions loader and boilerplate
```

```
> custom_engine/vendor/lua
Scripting
```

```
> custom_engine/vendor/stb
PNG loading
```

```
> /vendor/premake
A build configuration tool
```

# References studied
* https://github.com/etodd/lasercrabs
* https://github.com/Marzac/le3d

[Markdown](https://www.markdownguide.org/basic-syntax/)
