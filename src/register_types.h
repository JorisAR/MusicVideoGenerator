#ifndef MUSIC_VIDEO_GENERATOR_REGISTER_TYPES_H
#define MUSIC_VIDEO_GENERATOR_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_music_video_generator_module(ModuleInitializationLevel p_level);
void uninitialize_music_video_generator_module(ModuleInitializationLevel p_level);

#endif