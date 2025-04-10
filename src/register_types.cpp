#include "register_types.h"
#include "ray_marching/ray_marching_camera.h"

using namespace godot;

void initialize_music_video_generator_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
    {

        GDREGISTER_CLASS(RayMarchingCamera);
        UtilityFunctions::print("Music Video Generator module initialized.");

    }
}

void uninitialize_music_video_generator_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
    {
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT music_video_generator_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                              const GDExtensionClassLibraryPtr p_library,
                                                              GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_music_video_generator_module);
        init_obj.register_terminator(uninitialize_music_video_generator_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}