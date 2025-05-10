#include "animation_rig.h"
#include "animation_sequence.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

AnimationRig::AnimationRig() : current_sequence_index(0) {
}

AnimationRig::~AnimationRig() {
}

void AnimationRig::_notification(int p_what) {
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

void AnimationRig::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_song_settings", "settings"), &AnimationRig::set_song_settings);
    ClassDB::bind_method(D_METHOD("get_song_settings"), &AnimationRig::get_song_settings);
    ClassDB::bind_method(D_METHOD("set_animation_sequences", "seqs"), &AnimationRig::set_animation_sequences);
    ClassDB::bind_method(D_METHOD("get_animation_sequences"), &AnimationRig::get_animation_sequences);
    ClassDB::bind_method(D_METHOD("get_envelope_generators"), &AnimationRig::get_envelope_generators);
    ClassDB::bind_method(D_METHOD("set_envelope_generators", "value"), &AnimationRig::set_envelope_generators);

    ClassDB::bind_method(D_METHOD("get_randomize_order"), &AnimationRig::get_randomize_order);
    ClassDB::bind_method(D_METHOD("set_randomize_order", "value"), &AnimationRig::set_randomize_order);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "song_settings", PROPERTY_HINT_RESOURCE_TYPE, "SongSettings"),
                 "set_song_settings", "get_song_settings");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "animation_sequences", PROPERTY_HINT_TYPE_STRING, String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":AnimationSequence"),
                 "set_animation_sequences", "get_animation_sequences");

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "randomize_sequence_order"), "set_randomize_order", "get_randomize_order");
}

void AnimationRig::init() {
    current_sequence_index = 0;
    start_time = 0.0f;
    elapsed_time = 0.0f;
    global_sequence_index = 0;
    randomize_order = false;
    rng.instantiate();
}

void AnimationRig::update(const float delta) {
    if (song_settings.is_null()) {
        UtilityFunctions::print("Song settings not set!");
        return;
    }

    if (animation_sequences.size() == 0)
        return;

    float bpm = song_settings->get_bpm();
    float beats_per_bar = song_settings->get_time_signature_numerator();
    float seconds_per_bar = (60.0f / bpm) * beats_per_bar * (song_settings->get_time_signature_denominator() / 4.0f);

    Ref<AnimationSequence> active_seq = animation_sequences[current_sequence_index];
    float sequence_duration = active_seq->get_duration_bars() * seconds_per_bar;
    float progress = (elapsed_time - start_time) / sequence_duration;
    active_seq->process(this, delta, progress);     

    if (progress > 1.0f) {
        if(randomize_order) {
            current_sequence_index = rng->randi_range(0, animation_sequences.size() - 1) % animation_sequences.size();
        } else {
            current_sequence_index = (current_sequence_index + 1) % animation_sequences.size();
        }
        global_sequence_index++;
        start_time += sequence_duration;
        active_seq = animation_sequences[current_sequence_index];
        active_seq->init();
    }

    elapsed_time += delta;
}

void AnimationRig::set_song_settings(const Ref<SongSettings> &settings) {
    song_settings = settings;
}

Ref<SongSettings> AnimationRig::get_song_settings() const {
    return song_settings;
}

TypedArray<Ref<AnimationSequence>> AnimationRig::get_animation_sequences() const {
    return animation_sequences;
}

void AnimationRig::set_randomize_order(const bool value)
{
    randomize_order = value;
}

bool AnimationRig::get_randomize_order() const
{
    return randomize_order;
}

void AnimationRig::set_animation_sequences(const TypedArray<Ref<AnimationSequence>> &seqs) {
    animation_sequences = seqs;
}

TypedArray<Ref<EnvelopeGenerator>> AnimationRig::get_envelope_generators() const {
    // Implement envelope handling if needed.
    return TypedArray<Ref<EnvelopeGenerator>>();
}

void AnimationRig::set_envelope_generators(const TypedArray<Ref<EnvelopeGenerator>> value) {
    // Implement envelope handling if needed.
}
