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

# V8

Google's V8 is used as the primary scripting engine for JavaScript support. V8 is not distributed as a
submodule, but **depot_tools** is added instead. depot_tools can be used to fetch and compile V8 using gclient
and the Ninja compiler.

There is a batch script in **deps** named **build_v8.\<platform\>**, which automatically does all the fetching, syncing and
compilation for you. This is experimental and I'm not sure if this always works; but it's probably the easiest
way to guarantee a V8 build for Snuffbox.

As of current **Windows 10 SDK (ver. 10.0.15063.468)** is required to build V8 under MSVC.
The download archive for the SDKs can be found [here](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive).

# MSVC Version

The current working version has only been tested with MSVC 2017, 64-bit. The engine is being developed
in Visual Studio 2017 under platform tool set **v141**. Google's V8 has deprecated 2015 by now,
so it's highly advised to simply use 2017 when building the scripting environment as well.