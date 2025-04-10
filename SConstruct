#!/usr/bin/env python
import os
import sys
# from scons_compiledb import compile_db # Import the compile_db function # Call the compile_db function to enable compile_commands.json generation 
# compile_db()


# Import the SConstruct from godot-cpp
env = SConscript("godot-cpp/SConstruct")

# Add necessary include directories
env.Append(CPPPATH=[
    "src/gdcs/include/",
    "src/utility/",
    "src/",
    "src/ray_marching/"
])

# # Add main source files
sources = Glob("src/*.cpp") + Glob("src/utility/*.cpp") + Glob("src/gdcs/src/*.cpp") + \
      Glob("src/ray_marching/*.cpp")

#compiler flags
if env['PLATFORM'] == 'windows':
    if env['CXX'] == 'x86_64-w64-mingw32-g++':
        env.Append(CXXFLAGS=['-std=c++11'])  # Example flags for MinGW
    elif env['CXX'] == 'cl':
        env.Append(CXXFLAGS=['/EHsc'])  # Apply /EHsc for MSVC


# Handle different platforms
if env["platform"] == "macos":
    library = env.SharedLibrary(
        "project/addons/music_video_generator/bin/music_video_generator.{}.{}.framework/music_video_generator.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "project/addons/music_video_generator/bin/music_video_generator.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "project/addons/music_video_generator/bin/music_video_generator.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "project/addons/music_video_generator/bin/music_video_generator{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
