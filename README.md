# Summary
Study on C/C++, custom engines, games and stuff  

# How to work with this stuff
### __See sub-projects inside folder__
```
> __ workspace __
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
* Call GenerateProjects.bat
* Use your IDE of preference

> Target output into "/bin" folder  
> Intermediate output into "/bin-int" folder  

# References studied
* https://github.com/etodd/lasercrabs
* https://github.com/Marzac/le3d
