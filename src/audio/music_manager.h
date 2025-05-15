#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer_instance.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector4.hpp>
#include "audio/envelope_generators/envelope_generator.h"
#include "audio/song_settings.h"

using namespace godot;

class MusicManager : public Node3D {
    GDCLASS(MusicManager, Node3D);

public:

constexpr static int MAX_SPECTRUM_SIZE = 64;

    MusicManager();
    ~MusicManager();

    void set_song_settings(const Ref<SongSettings> &settings);
    Ref<SongSettings> get_song_settings() const;

    void set_audio_stream_player(AudioStreamPlayer* player);
    AudioStreamPlayer* get_audio_stream_player() const;

    TypedArray<Ref<EnvelopeGenerator>> get_envelope_generators() const;
    void set_envelope_generators(const TypedArray<Ref<EnvelopeGenerator>> value);

    Vector4 get_current_magnitude_data() const;
    Vector4 get_cumulative_magnitude_data() const;
    PackedFloat32Array get_spectrum_data() const;
    void set_spectrum_data(const PackedFloat32Array value);

    // Getters and setters for spectrum-related properties.

    void set_spectrum_size(int value);
    int get_spectrum_size() const;

    void set_spectrum_log_x_axis(bool value);
    bool get_spectrum_log_x_axis() const;

    void set_spectrum_db_octave_slope(float value);
    float get_spectrum_db_octave_slope() const;

    void set_spectrum_frequency_min(float value);
    float get_spectrum_frequency_min() const;

    void set_spectrum_frequency_max(float value);
    float get_spectrum_frequency_max() const;

protected:
    void _notification(int what);    
    static void _bind_methods();

private:
    void init();
    float apply_slope(float frequency, float value);
    void update_spectrum();
    void update(const float delta);

    // For the two-envelope detection approach:
    float fast_env = 0.0f;         // Fast envelope (quick tracking)
    float slow_env = 0.0f;         // Slow envelope (averaged energy)
    float transient_value = 0.0f;  // The final output envelope (1 if a transient is detected)
    
    //spectrum
    int spectrum_size = 64;
    bool spectrum_log_x_axis = true;
    float spectrum_db_octave_slope = 4.5f;
    float spectrum_frequency_min = 20.0f;
    float spectrum_frequency_max = 18000.0f;
    PackedFloat32Array spectrum_data;

    Vector4 current_magnitude_data;
    Vector4 cumulative_magnitude_data;
    AudioStreamPlayer* audio_stream_player = nullptr;
    TypedArray<Ref<EnvelopeGenerator>> envelope_generators;
    Ref<SongSettings> song_settings;

    Ref<AudioEffectSpectrumAnalyzerInstance> spectrum;
};

#endif // MUSIC_MANAGER_H
