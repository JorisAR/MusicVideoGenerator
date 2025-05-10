#ifndef ANIMATION_RIG_H
#define ANIMATION_RIG_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer_instance.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector4.hpp>
#include "audio/envelope_generators/envelope_generator.h"
#include "audio/song_settings.h"
#include "animation_sequence.h"

using namespace godot;


class AnimationRig : public Node3D {
    GDCLASS(AnimationRig, Node3D);

public:
    AnimationRig();
    ~AnimationRig();

    // Initializes the active animation sequence.
    void init();

    // delta is in seconds.
    void update(const float delta);

    void set_song_settings(const Ref<SongSettings> &settings);
    Ref<SongSettings> get_song_settings() const;

    TypedArray<Ref<EnvelopeGenerator>> get_envelope_generators() const;
    void set_envelope_generators(const TypedArray<Ref<EnvelopeGenerator>> value);

    // Get/set the list of animation sequences.
    void set_animation_sequences(const TypedArray<Ref<AnimationSequence>> &seqs);
    TypedArray<Ref<AnimationSequence>> get_animation_sequences() const;

    void set_randomize_order(const bool value);
    bool get_randomize_order() const;

protected:
    void _notification(int what);
    static void _bind_methods();

private:
    // The set of animation sequences to process.
    TypedArray<Ref<AnimationSequence>> animation_sequences;
    // Song settings used to compute timing
    Ref<SongSettings> song_settings;

    // Internals to track which animation sequence is active.
    int current_sequence_index = 0;
    int global_sequence_index = 0;
    float elapsed_time;
    float start_time;
    

    bool randomize_order = false;

    Ref<RandomNumberGenerator> rng;
};

#endif // ANIMATION_RIG_H
