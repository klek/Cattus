# Muggy Engine
MuggyEngine is a game engine project currently developed as a hobby, 
with the intention of being multiplatform (at least linux + Windows).
It currently uses GLFW for window management as this allows for quicker
progress but has the possibility to also support platform specific
implementations due to the current abstraction of functionality.

MuggyEngine is currently heavily inspired by **Primal** engine developed
by the channel **[Game Engine Series](https://www.youtube.com/@GameEngineSeries)** on youtube 

# Setup for Vulkan
Vulkan is required in order to build MuggyEngine. The recommended 
approach to getting vulkan development up and running is to use the
vulkan-sdk from lunarg. Please use the installation instructions for
your platform when installing the SDK. The current vulkan-sdk version
that this project is using is **1.3.275** and can be downloaded from here:
https://vulkan.lunarg.com/sdk/home

Remember to add vulkan to appropriate paths if necessary such that
includes of header files and libraries can be used without modifying
project-files.

# Premake
MuggyEngine currently uses premake for generating build-files on linux
and Windows. To generate the appropriate build-files for these 
platforms, one of the scripts in the premake-folder needs to be run.

## Windows
For Windows, running `generate_for_vs2022.bat` should generate the
appropriate build-files and solutions-files for Visual Studio 2022.
Then simply open the MuggySolution.sln in the root-folder and Visual
Studio should take care of the rest.

## Linux
For linux, first make sure the file `generate_for_linux.sh` has the 
executable flag by running:
```chmod +x generate_for_linux.sh```
Then running the file should generate the build-files for make, such
that they can either be used via command-line or in your editor of
choice.