# snuffbox-hydra
The "Hydra" revision of Snuffbox. Constantly getting new ideas over time, thus creating a new Snuffbox version without finishing the older ones.

# Compilation

CMake is basically required to set up this project. Everything has been tested with **CMake 3.10.1**.
Most dependencies are in the submodules of this project and are built alongside the source of the engine.

# Qt5

Qt5 is used for the interfaces for the tools. This means that Qt5 is required to be installed on the system
to build these corresponding projects. Deployment of the shared libraries also has to be done manually.

The current working version has been tested with **Qt 5.10.1**.

**CMAKE_PREFIX_PATH** has to contain the Qt5 installation path. For instance (for 5.10.1)
the prefix path should contain **DriveLetter:/QtInstallationDir/5.10.1/msvc2017_64** on Windows.

One of the primary reasons this is the case is because Qt5 is licensed under LGPL and require me to provide
a way to relink the actual libraries used. Aside from that, statically linking Qt5 using CMake is not fully
supported.

# MSVC Version

The current working version has only been tested with MSVC 2017, 64-bit. The engine is being developed
in Visual Studio 2017 under platform tool set **v141**.

# Clang

Snuffbox currently builds on Linux succesfully. The engine on Linux is developed in QtCreator using
**clang version 3.8.0-2ubuntu4 - x86_64-pc-linux-gnu** as compiler. The primary reason to use Clang
over GCC is because EASTL throws a *-Wliteral-suffix* under GCC, but *-w* is enabled for treating
warnings as errors. It doesn't happen when compiling EASTL, but when *\<EASTL/string.h\>* is included.
