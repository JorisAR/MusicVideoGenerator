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


Vector4 MusicManager::get_current_magnitude_data() const {
    return current_magnitude_data;
}

Vector4 MusicManager::get_cumulative_magnitude_data() const {
    return cumulative_magnitude_data;
}

PackedFloat32Array MusicManager::get_spectrum_data() const
{
    return spectrum_data;
}

void MusicManager::set_spectrum_data(const PackedFloat32Array value)
{
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

    ADD_GROUP("Spectrum Settings", "spectrum_");
    ClassDB::bind_method(D_METHOD("set_spectrum_size", "value"), &MusicManager::set_spectrum_size);
    ClassDB::bind_method(D_METHOD("get_spectrum_size"), &MusicManager::get_spectrum_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "spectrum_size"), "set_spectrum_size", "get_spectrum_size");

    ClassDB::bind_method(D_METHOD("set_spectrum_log_x_axis", "value"), &MusicManager::set_spectrum_log_x_axis);
    ClassDB::bind_method(D_METHOD("get_spectrum_log_x_axis"), &MusicManager::get_spectrum_log_x_axis);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "spectrum_log_x_axis"), "set_spectrum_log_x_axis", "get_spectrum_log_x_axis");

    ClassDB::bind_method(D_METHOD("set_spectrum_db_octave_slope", "value"), &MusicManager::set_spectrum_db_octave_slope);
    ClassDB::bind_method(D_METHOD("get_spectrum_db_octave_slope"), &MusicManager::get_spectrum_db_octave_slope);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "spectrum_db_octave_slope"), "set_spectrum_db_octave_slope", "get_spectrum_db_octave_slope");

    ClassDB::bind_method(D_METHOD("set_spectrum_frequency_min", "value"), &MusicManager::set_spectrum_frequency_min);
    ClassDB::bind_method(D_METHOD("get_spectrum_frequency_min"), &MusicManager::get_spectrum_frequency_min);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "spectrum_frequency_min"), "set_spectrum_frequency_min", "get_spectrum_frequency_min");

    ClassDB::bind_method(D_METHOD("set_spectrum_frequency_max", "value"), &MusicManager::set_spectrum_frequency_max);
    ClassDB::bind_method(D_METHOD("get_spectrum_frequency_max"), &MusicManager::get_spectrum_frequency_max);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "spectrum_frequency_max"), "set_spectrum_frequency_max", "get_spectrum_frequency_max");

    // Bind spectrum-related functions.
    ClassDB::bind_method(D_METHOD("set_spectrum_data", "value"), &MusicManager::set_spectrum_data);
    ClassDB::bind_method(D_METHOD("get_spectrum_data"), &MusicManager::get_spectrum_data);
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "spectrum_data"), "set_spectrum_data", "get_spectrum_data");

    ClassDB::bind_method(D_METHOD("get_current_magnitude_data"), &MusicManager::get_current_magnitude_data);
    ClassDB::bind_method(D_METHOD("get_cumulative_magnitude_data"), &MusicManager::get_cumulative_magnitude_data);
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

    //init spectrum
    spectrum_data.resize(spectrum_size);

    int count = envelope_generators.size();
    for (int i = 0; i < std::min(count, 4); ++i) {
        Ref<EnvelopeGenerator> generator = envelope_generators[i];
        if (generator.is_valid()) {
            generator->init(song_settings); 
        }
    }
}

float MusicManager::apply_slope(float frequency, float value) {
    const float gain_per_octave = std::pow(10, 4.5 / 20.0);
    float octaves_above = std::log2(frequency / spectrum_frequency_min);
    float gain = std::pow(gain_per_octave, octaves_above);
    return value * gain;
}

void MusicManager::update_spectrum() {
    const float MIN_DB = 60.f;
    const int barcount = spectrum_data.size();
    float prev_f = spectrum_frequency_min;

    for (size_t i = 0; i < barcount; i++)
    {
        float f = 0.0f;
        if(spectrum_log_x_axis)
            f = spectrum_frequency_min * std::pow(2.0f, std::log2(spectrum_frequency_max / spectrum_frequency_min) * float(i + 1) / float(barcount));
        else
            f = (1 + i) * (spectrum_frequency_max - spectrum_frequency_min) / barcount + spectrum_frequency_min;
        float magnitude = spectrum->get_magnitude_for_frequency_range(prev_f, f, AudioEffectSpectrumAnalyzerInstance::MAGNITUDE_MAX).length();
        prev_f = f;
        magnitude = apply_slope(f, magnitude);        
        // spectrum_data[i] = (MIN_DB + static_cast<float>(UtilityFunctions::linear_to_db(magnitude)) / MIN_DB);//Math::clamp(, 0.f, 1.f);
        magnitude = Math::clamp((MIN_DB + static_cast<float>(UtilityFunctions::linear_to_db(magnitude))) / MIN_DB, 0.f, 1.f);

        if (magnitude > spectrum_data[i])
            spectrum_data[i] = magnitude;
        else
            spectrum_data[i] = Math::lerp(spectrum_data[i], magnitude, 0.1f);        
    }    
}

//
// This process function is called on every frame.
//
void MusicManager::update(const float delta) {
    if (!audio_stream_player || !spectrum.is_valid()) {
        return;
    }

    update_spectrum();

    int count =  envelope_generators.size();
    for (int i = 0; i < std::min(count, 4); ++i) {
        Ref<EnvelopeGenerator> generator = envelope_generators[i];
        if (generator.is_valid() && i < 4) {
            current_magnitude_data[i] = generator->process(delta); 
        }
        cumulative_magnitude_data[i] += current_magnitude_data[i] * delta;
    }
}

void MusicManager::set_spectrum_size(int value)
{
    spectrum_size = Math::clamp(value, 0, MAX_SPECTRUM_SIZE);
}

int MusicManager::get_spectrum_size() const
{
    return spectrum_size;
}

void MusicManager::set_spectrum_log_x_axis(bool value) {
    spectrum_log_x_axis = value;
}

bool MusicManager::get_spectrum_log_x_axis() const {
    return spectrum_log_x_axis;
}

void MusicManager::set_spectrum_db_octave_slope(float value) {
    spectrum_db_octave_slope = value;
}

float MusicManager::get_spectrum_db_octave_slope() const {
    return spectrum_db_octave_slope;
}

void MusicManager::set_spectrum_frequency_min(float value) {
    spectrum_frequency_min = value;
}

float MusicManager::get_spectrum_frequency_min() const {
    return spectrum_frequency_min;
}

void MusicManager::set_spectrum_frequency_max(float value) {
    spectrum_frequency_max = value;
}

float MusicManager::get_spectrum_frequency_max() const {
    return spectrum_frequency_max;
}