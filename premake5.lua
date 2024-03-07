-- Creating the workspace
workspace "MuggySolution"
    configurations 
    { 
        "Debug", 
        "Release" 
    }
    -- NOTE(klek): Architecture cannot be in a table!
    architecture "x64"
    flags 
    { 
        "MultiProcessorCompile" 
    }
    
    if _TARGET_OS == "windows" then
        startproject "muggyExample"
    else
        startproject "muggyExample"
    end
    
    filter "configurations:Debug"
        symbols "on"

    filter "configurations:Release"
        symbols "on"
        --omitframepointer "On"
        flags 
        { 
            "LinkTimeOptimization", 
            "NoBufferSecurityCheck", 
            "NoRuntimeChecks" 
        }
    filter {}
        
    -- Set toolset to GCC on linux
    if _TARGET_OS == "linux" then
--        toolset "gcc"
    end
        
    -- Setup output directories -> Generate a bin folder in the root directory
    outputDir = "%{wks.location}/bin/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
    intermediateDir = "%{wks.location}/build/%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"
    
-- NOTE(klek): Consider moving each project to its own subfolder
project "muggy"
    location "muggy"
    kind "StaticLib"
    staticruntime "off"
    language "C++"
    cppdialect "C++17"
    targetname "%{prj.name}"
    -- NOTE(klek): Consider to explicitly state which files should be
    --             included
    files 
    { 
        "%{prj.name}/**.h", 
        "%{prj.name}/**.cpp" 
    }
    removefiles
    {
        "%{prj.name}/thirdparty/**.cpp",
        "%{prj.name}/thirdparty/**.c",
        "%{prj.name}/thirdparty/**.h",
    }
    includedirs 
    { 
        "%{wks.location}/muggy", 
        ( outputDir .. "/glfw/include" ) 
    }
    libdirs
    {
        ( outputDir .. "/glfw/lib" )
    }
    if _TARGET_OS == "windows" then 
        links 
        { 
            "glfw3"
        }
    else
--        buildoptions 
--        { 
--            "-Wno-switch",
--            "-Wno-missing-field-initializers",
--            "-Wno-unused-parameter", 
--            "-Wno-ignored-qualifiers", 
--            "-Wno-unknown-pragmas", 
--            "-Wno-class-memaccess", 
--            "-Wno-reorder" 
--        }
        links
        {
            "glfw3",
            "X11"
        }
    end
    targetdir (outputDir)
    objdir ( "!" .. intermediateDir .. "/%{prj.name}" )
--    rtti "off"
--    floatingpoint "fast"
--    conformancemode "on"
--    exceptionhandling "off"
--    warnings "Extra"
    -- TODO(klek): Add prebuild action to build and copy thirdparty library 
    --             outputs to outputDir
    -- Setup glfw dir
--    glfwDir = "%{prj.location}/thirdparty/glfw"
    thirdpartyDir = "%{prj.location}/thirdparty"
    if _TARGET_OS == "windows" then
        if _ACTION == "vs2022" then
            prebuildcommands( "%{prj.location}/cmake_glfw_on_windows.bat %{thirdpartyDir} %{intermediateDir} %{cfg.targetdir}" )
            prebuildcommands( "%{prj.location}/cmake_vulkan_build_on_windows.bat %{thirdpartyDir} %{intermediateDir} %{cfg.targetdir}" )
        end
    else
        prebuildcommands( "./%{prj.location}/cmake_glfw_on_linux.sh %{thirdpartyDir} %{intermediateDir} %{outputDir}" )
        prebuildcommands( "./%{prj.location}/cmake_vulkan_build_on_linux.sh %{thirdpartyDir} %{intermediateDir} %{outputDir}" )
    end
    filter "configurations:Debug"
        defines 
        {
            "DEBUG_BUILD", 
            "GLFW" 
        }

    filter "configurations:Release"
        defines 
        { 
            "GLFW" 
        }
    filter {}


-- NOTE(klek): Consider moving each project to its own subfolder
--project "muggyLib"
--    location "muggyLib"
--    kind "SharedLib"
--    language "C++"
--    cppdialect "C++17"
--    defines
--    {
--        "BUILD_SHARED_LIBS==ON"
--    }
    
-- NOTE(klek): Consider moving each project to its own subfolder
--project "muggyEditor"
--    location "muggyEditor"
--    kind "WindowedApp"
--    language "C++"
--    cppdialect "C++17"

-- NOTE(klek): Consider moving each project to its own subfolder
project "muggyExample"
    location "muggyExample"
--    kind "WindowedApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    targetname "%{prj.name}"
    -- NOTE(klek): Consider to explicitly state which files should be
    --             included
    files
    {
        "%{prj.name}/**.h",
        "%{prj.name}/**.cpp",
    }
    includedirs
    {
        "%{wks.location}/muggyExample",
--        "%{wks.location}/muggyExample/tests",
        "%{wks.location}/muggy",
--        "%{wks.location}/muggy/platform",
        ( outputDir .. "/glfw/include" ) 
    }
    libdirs
    {
--        (outputDir)
        ( outputDir .. "/glfw/lib" ) 
    }
    if _TARGET_OS == "windows" then
        links
        {
            "muggy",
            "glfw3"
        }
    else
--        buildoptions 
--        { 
--            "-Wno-switch",
--            "-Wno-missing-field-initializers",
--            "-Wno-unused-parameter", 
--            "-Wno-ignored-qualifiers", 
--            "-Wno-unknown-pragmas", 
--            "-Wno-class-memaccess", 
--            "-Wno-reorder" 
--        }
    links
        {
            "muggy",
            "glfw3",
            "X11"
        }
    end    
    targetdir (outputDir)
    objdir ( "!" .. intermediateDir .. "/%{prj.name}" )
--    rtti "off"
--    floatingpoint "fast"
--    conformancemode "on"
--    exceptionhandling "off"
--    warnings "Extra"
    dependson "muggy"
    filter "configurations:Debug"
        defines 
        { 
            "DEBUG_BUILD",
            "GLFW" 
        }

    filter "configurations:Release"
        defines 
        { 
            "GLFW" 
        }
    filter {}

--include "muggy/thirdparty/glfw.lua"
