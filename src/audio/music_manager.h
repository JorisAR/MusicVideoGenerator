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
    MusicManager();
    ~MusicManager();

    // Called every frame; here we update our frequency range data.
    void init();
    void update(const float delta);

    void set_song_settings(const Ref<SongSettings> &settings);
    Ref<SongSettings> get_song_settings() const;

    void set_audio_stream_player(AudioStreamPlayer* player);
    AudioStreamPlayer* get_audio_stream_player() const;

    TypedArray<Ref<EnvelopeGenerator>> get_envelope_generators() const;
    void set_envelope_generators(const TypedArray<Ref<EnvelopeGenerator>> value);

    // The spectrum analyzer instance:
    Ref<AudioEffectSpectrumAnalyzerInstance> spectrum;

    // Our envelope output; currently,
    // raw_magnitude_data[0] holds the binary transient envelope.
    Vector4 get_raw_magnitude_data() const;

    

protected:
    void _notification(int what);
    static void _bind_methods();

private:
    // For the two-envelope detection approach:
    float fast_env = 0.0f;         // Fast envelope (quick tracking)
    float slow_env = 0.0f;         // Slow envelope (averaged energy)
    float transient_value = 0.0f;  // The final output envelope (1 if a transient is detected)

    Vector4 raw_magnitude_data;
    AudioStreamPlayer* audio_stream_player = nullptr;
    TypedArray<Ref<EnvelopeGenerator>> envelope_generators;
    Ref<SongSettings> song_settings;
};

#endif // MUSIC_MANAGER_H
