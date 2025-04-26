#include "dual_envelope_transient_generator.h"

using namespace godot;

#include "dual_envelope_transient_generator.h"

using namespace godot;

void DualEnvelopeTransientGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_fast_attack_alpha", "p_val"), &DualEnvelopeTransientGenerator::set_fast_attack_alpha);
    ClassDB::bind_method(D_METHOD("get_fast_attack_alpha"), &DualEnvelopeTransientGenerator::get_fast_attack_alpha);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fast_attack_alpha", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), 
                 "set_fast_attack_alpha", "get_fast_attack_alpha");


    ClassDB::bind_method(D_METHOD("set_fast_release_alpha", "p_val"), &DualEnvelopeTransientGenerator::set_fast_release_alpha);
    ClassDB::bind_method(D_METHOD("get_fast_release_alpha"), &DualEnvelopeTransientGenerator::get_fast_release_alpha);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fast_release_alpha", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), 
                 "set_fast_release_alpha", "get_fast_release_alpha");


    ClassDB::bind_method(D_METHOD("set_slow_alpha", "p_val"), &DualEnvelopeTransientGenerator::set_slow_alpha);
    ClassDB::bind_method(D_METHOD("get_slow_alpha"), &DualEnvelopeTransientGenerator::get_slow_alpha);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "slow_alpha", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), 
                 "set_slow_alpha", "get_slow_alpha");


    ClassDB::bind_method(D_METHOD("set_threshold_delta", "p_val"), &DualEnvelopeTransientGenerator::set_threshold_delta);
    ClassDB::bind_method(D_METHOD("get_threshold_delta"), &DualEnvelopeTransientGenerator::get_threshold_delta);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshold_delta", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), 
                 "set_threshold_delta", "get_threshold_delta");
    ClassDB::bind_method(D_METHOD("set_transient_decay_rate", "p_val"), &DualEnvelopeTransientGenerator::set_transient_decay_rate);
    ClassDB::bind_method(D_METHOD("get_transient_decay_rate"), &DualEnvelopeTransientGenerator::get_transient_decay_rate);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "transient_decay_rate", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"), 
                 "set_transient_decay_rate", "get_transient_decay_rate");
}


DualEnvelopeTransientGenerator::DualEnvelopeTransientGenerator() {

}

void DualEnvelopeTransientGenerator::init(Ref<SongSettings> song_settings)
{
    fast_env = 0.0f;
    slow_env = 0.0f;
    transient_value = 0.0f;
    fast_attack_alpha = 0.3f;
    fast_release_alpha = 0.3f;
    slow_alpha = 0.01f;
    threshold_delta = 0.05f;
    transient_decay_rate = 0.05f;
}

float DualEnvelopeTransientGenerator::process(float delta)
{
    float audio_input = 0.0f;
    // Update the fast envelope: follow the input quickly.
    if (audio_input > fast_env) {
        fast_env = Math::lerp(fast_env, audio_input, fast_attack_alpha);
    } else {
        fast_env = Math::lerp(fast_env, audio_input, fast_release_alpha);
    }
    // Update the slow envelope: average the input slowly.
    slow_env = Math::lerp(slow_env, audio_input, slow_alpha);

    // If the fast envelope exceeds the slow one by a threshold and we're not already triggered,
    // then trigger the transient.
    if ((fast_env - slow_env) > threshold_delta && transient_value < 0.1f) {
        transient_value = 1.0f;
    }
    // Smoothly decay the transient trigger.
    transient_value = Math::lerp(transient_value, 0.0f, transient_decay_rate);
    return transient_value;
}

void DualEnvelopeTransientGenerator::set_fast_attack_alpha(float p_val) {
    fast_attack_alpha = p_val;
}

float DualEnvelopeTransientGenerator::get_fast_attack_alpha() const {
    return fast_attack_alpha;
}

void DualEnvelopeTransientGenerator::set_fast_release_alpha(float p_val) {
    fast_release_alpha = p_val;
}

float DualEnvelopeTransientGenerator::get_fast_release_alpha() const {
    return fast_release_alpha;
}

void DualEnvelopeTransientGenerator::set_slow_alpha(float p_val) {
    slow_alpha = p_val;
}

float DualEnvelopeTransientGenerator::get_slow_alpha() const {
    return slow_alpha;
}

void DualEnvelopeTransientGenerator::set_threshold_delta(float p_val) {
    threshold_delta = p_val;
}

float DualEnvelopeTransientGenerator::get_threshold_delta() const {
    return threshold_delta;
}

void DualEnvelopeTransientGenerator::set_transient_decay_rate(float p_val) {
    transient_decay_rate = p_val;
}

float DualEnvelopeTransientGenerator::get_transient_decay_rate() const {
    return transient_decay_rate;
}
