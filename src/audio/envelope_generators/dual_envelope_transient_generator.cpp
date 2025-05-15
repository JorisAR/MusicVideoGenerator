// dual_envelope_transient_generator.cpp
#include "dual_envelope_transient_generator.h"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "audio/song_settings.h"

using namespace godot;

void DualEnvelopeTransientGenerator::_bind_methods() {
    // Bus/effect properties.
    ClassDB::bind_method(D_METHOD("set_bus_id", "value"), &DualEnvelopeTransientGenerator::set_bus_id);
    ClassDB::bind_method(D_METHOD("get_bus_id"), &DualEnvelopeTransientGenerator::get_bus_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bus_id"), "set_bus_id", "get_bus_id");

    ClassDB::bind_method(D_METHOD("set_effect_id", "value"), &DualEnvelopeTransientGenerator::set_effect_id);
    ClassDB::bind_method(D_METHOD("get_effect_id"), &DualEnvelopeTransientGenerator::get_effect_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "effect_id"), "set_effect_id", "get_effect_id");

    // Frequency range properties.
    ClassDB::bind_method(D_METHOD("set_min_frequency", "value"), &DualEnvelopeTransientGenerator::set_min_frequency);
    ClassDB::bind_method(D_METHOD("get_min_frequency"), &DualEnvelopeTransientGenerator::get_min_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_frequency"), "set_min_frequency", "get_min_frequency");

    ClassDB::bind_method(D_METHOD("set_max_frequency", "value"), &DualEnvelopeTransientGenerator::set_max_frequency);
    ClassDB::bind_method(D_METHOD("get_max_frequency"), &DualEnvelopeTransientGenerator::get_max_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_frequency"), "set_max_frequency", "get_max_frequency");

    // Fast envelope properties.
    ClassDB::bind_method(D_METHOD("set_fast_attack", "value"), &DualEnvelopeTransientGenerator::set_fast_attack);
    ClassDB::bind_method(D_METHOD("get_fast_attack"), &DualEnvelopeTransientGenerator::get_fast_attack);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fast_attack"), "set_fast_attack", "get_fast_attack");

    ClassDB::bind_method(D_METHOD("set_fast_decay", "value"), &DualEnvelopeTransientGenerator::set_fast_decay);
    ClassDB::bind_method(D_METHOD("get_fast_decay"), &DualEnvelopeTransientGenerator::get_fast_decay);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fast_decay"), "set_fast_decay", "get_fast_decay");

    // Slow envelope properties.
    ClassDB::bind_method(D_METHOD("set_slow_attack", "value"), &DualEnvelopeTransientGenerator::set_slow_attack);
    ClassDB::bind_method(D_METHOD("get_slow_attack"), &DualEnvelopeTransientGenerator::get_slow_attack);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "slow_attack"), "set_slow_attack", "get_slow_attack");

    ClassDB::bind_method(D_METHOD("set_slow_decay", "value"), &DualEnvelopeTransientGenerator::set_slow_decay);
    ClassDB::bind_method(D_METHOD("get_slow_decay"), &DualEnvelopeTransientGenerator::get_slow_decay);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "slow_decay"), "set_slow_decay", "get_slow_decay");

    // Detection threshold.
    ClassDB::bind_method(D_METHOD("set_detection_threshold", "value"), &DualEnvelopeTransientGenerator::set_detection_threshold);
    ClassDB::bind_method(D_METHOD("get_detection_threshold"), &DualEnvelopeTransientGenerator::get_detection_threshold);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "detection_threshold"), "set_detection_threshold", "get_detection_threshold");

    // Output envelope properties.
    ClassDB::bind_method(D_METHOD("set_output_attack", "value"), &DualEnvelopeTransientGenerator::set_output_attack);
    ClassDB::bind_method(D_METHOD("get_output_attack"), &DualEnvelopeTransientGenerator::get_output_attack);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "output_attack"), "set_output_attack", "get_output_attack");

    ClassDB::bind_method(D_METHOD("set_output_decay", "value"), &DualEnvelopeTransientGenerator::set_output_decay);
    ClassDB::bind_method(D_METHOD("get_output_decay"), &DualEnvelopeTransientGenerator::get_output_decay);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "output_decay"), "set_output_decay", "get_output_decay");
}

DualEnvelopeTransientGenerator::DualEnvelopeTransientGenerator() {
    bus_id = 0;
    effect_id = 0;
    min_frequency = 20.0f;
    max_frequency = 20000.0f;

    // Fast envelope: quick to react.
    fast_attack = 0.05f;
    fast_decay = 0.05f;

    // Slow envelope: more averaged response.
    slow_attack = 0.3f;
    slow_decay = 0.3f;

    // The fast–slow difference must exceed this value to trigger a transient.
    detection_threshold = 0.1f;

    // Output transient parameters.
    output_attack = 0.01f;  // Nearly instantaneous rise.
    output_decay = 0.3f;    // Gradual decay.

    envelope_fast = 0.0f;
    envelope_slow = 0.0f;
    transient = 0.0f;
}

void DualEnvelopeTransientGenerator::set_bus_id(int value) {
    bus_id = value;
}
int DualEnvelopeTransientGenerator::get_bus_id() const {
    return bus_id;
}

void DualEnvelopeTransientGenerator::set_effect_id(int value) {
    effect_id = value;
}
int DualEnvelopeTransientGenerator::get_effect_id() const {
    return effect_id;
}

void DualEnvelopeTransientGenerator::set_min_frequency(float value) {
    min_frequency = value;
}
float DualEnvelopeTransientGenerator::get_min_frequency() const {
    return min_frequency;
}

void DualEnvelopeTransientGenerator::set_max_frequency(float value) {
    max_frequency = value;
}
float DualEnvelopeTransientGenerator::get_max_frequency() const {
    return max_frequency;
}

void DualEnvelopeTransientGenerator::set_fast_attack(float value) {
    fast_attack = value;
}
float DualEnvelopeTransientGenerator::get_fast_attack() const {
    return fast_attack;
}

void DualEnvelopeTransientGenerator::set_fast_decay(float value) {
    fast_decay = value;
}
float DualEnvelopeTransientGenerator::get_fast_decay() const {
    return fast_decay;
}

void DualEnvelopeTransientGenerator::set_slow_attack(float value) {
    slow_attack = value;
}
float DualEnvelopeTransientGenerator::get_slow_attack() const {
    return slow_attack;
}

void DualEnvelopeTransientGenerator::set_slow_decay(float value) {
    slow_decay = value;
}
float DualEnvelopeTransientGenerator::get_slow_decay() const {
    return slow_decay;
}

void DualEnvelopeTransientGenerator::set_detection_threshold(float value) {
    detection_threshold = value;
}
float DualEnvelopeTransientGenerator::get_detection_threshold() const {
    return detection_threshold;
}

void DualEnvelopeTransientGenerator::set_output_attack(float value) {
    output_attack = value;
}
float DualEnvelopeTransientGenerator::get_output_attack() const {
    return output_attack;
}

void DualEnvelopeTransientGenerator::set_output_decay(float value) {
    output_decay = value;
}
float DualEnvelopeTransientGenerator::get_output_decay() const {
    return output_decay;
}

void DualEnvelopeTransientGenerator::init(Ref<SongSettings> song_settings) {
    // Obtain the spectrum analyzer instance from AudioServer.
    if (!AudioServer::get_singleton())
        return;
    
    int bus_count = AudioServer::get_singleton()->get_bus_count();
    if (bus_id < 0 || bus_id >= bus_count)
        return;
    
    int effect_count = AudioServer::get_singleton()->get_bus_effect_count(bus_id);
    if (effect_id < 0 || effect_id >= effect_count)
        return;
    
    Ref<AudioEffect> effect = AudioServer::get_singleton()->get_bus_effect(bus_id, effect_id);
    if (effect.is_valid() && Object::cast_to<AudioEffectSpectrumAnalyzer>(*effect)) {
        spectrum = AudioServer::get_singleton()->get_bus_effect_instance(bus_id, effect_id);
    }
}

float DualEnvelopeTransientGenerator::process(float delta) {
    // If the spectrum analyzer isn't valid, simply decay the output transient.
    if (!spectrum.is_valid()) {
        transient = Math::lerp(transient, 0.0f, Math::clamp(delta / output_decay, 0.0f, 1.0f));
        return transient;
    }

    // Obtain the audio magnitude for the defined frequency range.
    float magnitude = spectrum->get_magnitude_for_frequency_range(min_frequency, max_frequency,
                                                            AudioEffectSpectrumAnalyzerInstance::MAGNITUDE_MAX).length();
    const float MIN_DB = 60.0f;
    magnitude =
        Math::clamp((MIN_DB + static_cast<float>(UtilityFunctions::linear_to_db(magnitude))) / MIN_DB, 0.f, 1.f);

    // Update the fast envelope.
    if (magnitude > envelope_fast) {
        envelope_fast = Math::lerp(envelope_fast, magnitude, Math::clamp(delta / fast_attack, 0.0f, 1.0f));
    } else {
        envelope_fast = Math::lerp(envelope_fast, magnitude, Math::clamp(delta / fast_decay, 0.0f, 1.0f));
    }

    // Update the slow envelope.
    if (magnitude > envelope_slow) {
        envelope_slow = Math::lerp(envelope_slow, magnitude, Math::clamp(delta / slow_attack, 0.0f, 1.0f));
    } else {
        envelope_slow = Math::lerp(envelope_slow, magnitude, Math::clamp(delta / slow_decay, 0.0f, 1.0f));
    }

    // Compute the difference between fast and slow envelopes.
    float diff = envelope_fast - envelope_slow;

    // If the difference is large enough, trigger the transient output.
    if (diff > detection_threshold) {
        transient = Math::lerp(transient, 1.0f, Math::clamp(delta / output_attack, 0.0f, 1.0f));
    } else {
        transient = Math::lerp(transient, 0.0f, Math::clamp(delta / output_decay, 0.0f, 1.0f));
    }

    transient = Math::clamp(transient, 0.0f, 1.0f);
    return transient;
}
