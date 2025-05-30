#include "register_types.h"
#include "ray_marching/ray_marching_camera.h"
#include "audio/song_settings.h"
#include "audio/music_manager.h"
#include "audio/envelope_generators/envelope_generator.h"
#include "audio/envelope_generators/transient_envelope_generator.h"
#include "audio/envelope_generators/beat_sequencer_envelope.h"
#include "audio/envelope_generators/lfo_envelope_generator.h"
#include "audio/envelope_generators/amplitude_envelope_generator.h"
#include "audio/envelope_generators/dual_envelope_transient_generator.h"
#include "animation_rig/animation_rig.h"
#include "animation_rig/animation_sequence.h"
#include "animation_rig/animation_sequence_move.h"

using namespace godot;

void initialize_music_video_generator_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
    {

        //Music Managing
        GDREGISTER_CLASS(SongSettings);
        GDREGISTER_CLASS(MusicManager);

        //generators
        GDREGISTER_ABSTRACT_CLASS(EnvelopeGenerator);
        GDREGISTER_CLASS(TransientEnvelopeGenerator);
        GDREGISTER_CLASS(BeatSequencerEnvelope);
        GDREGISTER_CLASS(LFOEnvelopeGenerator);
        GDREGISTER_CLASS(AmplitudeEnvelopeGenerator);
        GDREGISTER_CLASS(DualEnvelopeTransientGenerator);
        // UtilityFunctions::print("Music Video Generator module initialized.");

        //Rendering
        GDREGISTER_CLASS(RayMarchingCamera);

        //Animation rig
        GDREGISTER_ABSTRACT_CLASS(AnimationSequence);
        GDREGISTER_CLASS(AnimationSequenceMove);
        GDREGISTER_CLASS(AnimationRig);
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