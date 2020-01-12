# Summary
Provide somewhat crossplatform set of interfaces  

# Sources
### Compilers
Code  
```
> macros
MSVC: _MSC_VER
      _MSC_FULL_VER, _MSC_BUILD

GCC: __GNUC__
     __GNUC_MINOR__, __GNUC_PATCHLEVEL__

MINGW: __MINGW64__ || __MINGW32__
       __MINGW32_MAJOR_VERSION, __MINGW32_MINOR_VERSION
       __MINGW64_MAJOR_VERSION, __MINGW64_MINOR_VERSION

LLVM/Clang: __clang__ && __llvm__
            __clang_major__, __clang_minor__, __clang_patchlevel__, __clang_version__
```

Links  
* https://sourceforge.net/p/predef/wiki/Compilers/
* https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?redirectedfrom=MSDN&view=vs-2019
* https://devblogs.microsoft.com/cppblog/side-by-side-minor-version-msvc-toolsets-in-visual-studio-2017/
* https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
* https://gcc.gnu.org/wiki/Visibility
* https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
* https://clang.llvm.org/docs/LTOVisibility.html

### Operating Systems
Code  
```
> macros
Windows: _WIN64 || _WIN32
         optional?: __CYGWIN__

Apple: __APPLE__ && __MACH__
       #include <TargetConditionals.h>
       iPhone simulator: TARGET_IPHONE_SIMULATOR == 1
       iPhone:           TARGET_OS_IPHONE == 1
       MacOS:            TARGET_OS_MAC == 1

Android: __ANDROID__ // @Note: is a Linux platform
         #include <android/api-level.h>
         __ANDROID_API__

Linux: __linux__
```

Links  
* https://sourceforge.net/p/predef/wiki/OperatingSystems/
* https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
* http://www.faqs.org/docs/Linux-HOWTO/GCC-HOWTO.html

### Debug tools
Code  
```
> built-in breakpoints
MSVC: __debugbreak()
Windows: DebugBreak() // which you don't want to use, probably
Apple: __builtin_trap()
       might require?: #include <TargetConditionals.h>
Linux: raise(SIGTRAP)
       might require?: #include <signal.h>
GCC: __builtin_trap() // @Note: optimizes out any following non-conditional code
LLVM/Clang: __builtin_debugtrap()

> build-in function names
seems like it suffies for now to use this one: __FUNCTION__
also, I presume, there is a somehow less portable version: __func__
```

Links  
* https://github.com/scottt/debugbreak
* https://github.com/gpakosz/PPK_ASSERT
* https://docs.microsoft.com/en-us/cpp/intrinsics/debugbreak
* https://docs.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-debugbreak
* https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?redirectedfrom=MSDN&view=vs-2019
* https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html
* https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
* http://clang.llvm.org/docs/LanguageExtensions.html

### Data types
Code  
```
#include <stdint.h>
#include <float.h>
```

### Architectures
Links  
* https://sourceforge.net/p/predef/wiki/Architectures/
