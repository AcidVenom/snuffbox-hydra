# snuffbox-hydra
The "Hydra" revision of Snuffbox. Constantly getting new ideas over time, thus creating a new Snuffbox version without finishing the older ones.

# Cloning the repository
This revision can simply be cloned through `git clone https://github.com/AcidVenom/snuffbox-hydra`. However, there seems to be a small issue regarding the submodules of
EASTL. This causes an infinite recursion retrieving its corresponding "test" submodules. `--recurse-submodules` seems to completely break.

I have added a simple shell script in the root of this repository, namely **update_submodules.sh**, which updates the submodules for you one-by-one, avoiding this issue.

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

# Scripting

Currently duktape is implemented for JavaScript as the main scripting language. Scripting can however be
disabled for full C++ games. By setting **SNUFF_SCRIPTING** to **disabled**, *SNUFF_NSCRIPTING* will be defined
in the preprocessor, compiling away all scripting definitions. CMake will make sure to not include any script
related source files in the engine (apart from *src/scripting/script_class.h*). It is advised to do a full rebuild
when switching the scripting environment.

# Cross-platform status

Currently only OpenGL is implemented, OpenGL version **4.6** is required for SPIR-V support. The DirectX 11 implementation will soon follow. Please note that Intel's current integrated graphics drivers only support OpenGL **4.5** on Windows-based systems.

# Linux/Ubuntu

Snuffbox has been tested on both **Ubuntu 16.04 LTS** and **Ubuntu 18.04 LTS**, which both seem to be functioning correctly.

# CVars

The current available CVars are the following:

|Name     |Description                           |Default    |
|---------|--------------------------------------|-----------|
|r_vsync  |Should VSync be enabled?              |true       |
|r_width  |The width of the renderer in pixels   |1280       |
|r_height |The height of the renderer in pixels  |720        |

# Shader constants

The current shader implementation uses the following buffers (which will later be put into a *defines* shader)

**Available constant buffers**

|Constant buffer index |Description    |
|----------------------|---------------|
|b0                    |Per-frame data |
|b1                    |Per-object data|

**Constant buffer data**

*b0*

|Binding location     |Type     |Description                            |
|---------------------|---------|---------------------------------------|
|0                    |float4x4 |The projection matrix of the camera    |
|1                    |float4x4 |The view matrix of the camera          |
|2                    |float4x4 |The inverse of the projection * view   |
|3                    |float3   |The eye position of the camera         |
|4                    |float    |The current application time in seconds|

*b1*

|Binding location     |Type     |Description                            |
|---------------------|---------|---------------------------------------|
|0                    |float4x4 |The local-to-world matrix              |
|1                    |float4x4 |The projection * view * world matrix   |
|2                    |float4x4 |The world-to-local matrix              |

