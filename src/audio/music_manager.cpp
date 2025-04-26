#include "music_manager.h"
#include <algorithm>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MusicManager::MusicManager() {
    // Constructor initialization if needed.
    fast_env = 0.0f;
    slow_env = 0.0f;
    transient_value = 0.0f;
}

MusicManager::~MusicManager() {
    // Destructor cleanup if needed.
}

void MusicManager::set_audio_stream_player(AudioStreamPlayer* player) {
    audio_stream_player = player;
}

AudioStreamPlayer* MusicManager::get_audio_stream_player() const {
    return audio_stream_player;
}

TypedArray<Ref<EnvelopeGenerator>> MusicManager::get_envelope_generators() const
{
    return envelope_generators;
}

void MusicManager::set_envelope_generators(const TypedArray<Ref<EnvelopeGenerator>> value)
{
    envelope_generators = value;
}

void MusicManager::set_song_settings(const Ref<SongSettings> &settings) {
    song_settings = settings;
}

Ref<SongSettings> MusicManager::get_song_settings() const {
    return song_settings;
}


Vector4 MusicManager::get_raw_magnitude_data() const {
    return raw_magnitude_data;
}

void MusicManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_audio_stream_player", "player"), &MusicManager::set_audio_stream_player);
    ClassDB::bind_method(D_METHOD("get_audio_stream_player"), &MusicManager::get_audio_stream_player);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "audio_stream_player", PROPERTY_HINT_NODE_TYPE, "AudioStreamPlayer"),
                 "set_audio_stream_player", "get_audio_stream_player");

    ClassDB::bind_method(D_METHOD("set_song_settings", "settings"), &MusicManager::set_song_settings);
    ClassDB::bind_method(D_METHOD("get_song_settings"), &MusicManager::get_song_settings);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "song_settings", PROPERTY_HINT_RESOURCE_TYPE, "SongSettings"),
                 "set_song_settings", "get_song_settings");

    ClassDB::bind_method(D_METHOD("set_envelope_generators", "value"), &MusicManager::set_envelope_generators);
    ClassDB::bind_method(D_METHOD("get_envelope_generators"), &MusicManager::get_envelope_generators);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "envelope_generators", PROPERTY_HINT_TYPE_STRING, String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":EnvelopeGenerator"),
                 "set_envelope_generators", "get_envelope_generators");

}

void MusicManager::_notification(int p_what) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            set_process_internal(true);
            break;
        }
        case NOTIFICATION_EXIT_TREE: {
            set_process_internal(false);
            break;
        }
        case NOTIFICATION_READY: {
            init();
            break;
        }
        case NOTIFICATION_INTERNAL_PROCESS: {
            
            float delta = get_process_delta_time();
            update(delta);
            break;
        }
    }
}

void MusicManager::init() {
    if (!audio_stream_player) {
        UtilityFunctions::printerr("No AudioStreamPlayer assigned.");
        return;
    }
    auto server = AudioServer::get_singleton();
    int bus_index = server->get_bus_index(audio_stream_player->get_bus());
    int effect_count = server->get_bus_effect_count(bus_index);
    for (size_t i = 0; i < effect_count; i++) {
        Ref<AudioEffectInstance> effect = server->get_bus_effect_instance(bus_index, i);
        spectrum = Ref<AudioEffectSpectrumAnalyzerInstance>(
            Object::cast_to<AudioEffectSpectrumAnalyzerInstance>(*(effect)));
        if (spectrum.is_valid()) {
            break;
        }
    }
    if (spectrum.is_valid()) {
        UtilityFunctions::print("Spectrum analyzer found and initialized.");
    } else {
        UtilityFunctions::printerr("No valid spectrum analyzer found on the bus.");
    }

    for (int i = 0; i < envelope_generators.size(); ++i) {
        Ref<EnvelopeGenerator> generator = envelope_generators[i];
        if (generator.is_valid()) {
            generator->init(song_settings); 
        }
    }
}

//
// This process function is called on every frame.
//
void MusicManager::update(const float delta) {
    if (!audio_stream_player || !spectrum.is_valid()) {
        return;
    }
    int count = envelope_generators.size();
    for (int i = 0; i < envelope_generators.size(); ++i) {
        Ref<EnvelopeGenerator> generator = envelope_generators[i];
        if (generator.is_valid() && i < 4) {
            raw_magnitude_data[i] = generator->process(delta); 
        }
    }
    
    
    /*
    // Retrieve high-frequency energy (e.g., 16kHz to 20kHz).
    Vector2 data = spectrum->get_magnitude_for_frequency_range(16000, 20000);
    float raw_value = data.length();

    //
    // Update the fast envelope so that it quickly follows the raw high-freq changes.
    //
    const float fastAttackAlpha = 0.3f;   // Fast rise
    const float fastReleaseAlpha = 0.3f;  // Also a fast release for prompt tracking
    if (raw_value > fast_env) {
        fast_env = Math::lerp(fast_env, raw_value, fastAttackAlpha);
    } else {
        fast_env = Math::lerp(fast_env, raw_value, fastReleaseAlpha);
    }

    //
    // Update the slow envelope so that it follows changes very gradually.
    //
    const float slowAlpha = 0.1f; // Very slow adaptation: holds an average of the high frequencies.
    slow_env = Math::lerp(slow_env, raw_value, slowAlpha);

    //
    // Peak detection: if the fast envelope exceeds the slow envelope by a target difference,
    // trigger a transient detection.
    //
    const float thresholdDelta = 0.9f; // Change this value to tune detection sensitivity.
    const float d = Math::abs(fast_env - slow_env) / Math::abs(fast_env + slow_env);
    if (d > thresholdDelta) {
        transient_value = 1.0f;
    }

    //
    // Decay the transient detection envelope smoothly.
    //
    transient_value = Math::lerp(transient_value, 0.0f, 0.05f);

    //
    // Output the final envelope value.
    //
    raw_magnitude_data[0] = transient_value;*/
}
