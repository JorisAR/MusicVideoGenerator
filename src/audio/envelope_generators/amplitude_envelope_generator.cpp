#include "amplitude_envelope_generator.h"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/audio_bus_layout.hpp>
#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "audio/song_settings.h"

using namespace godot;

void AmplitudeEnvelopeGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_bus_id", "value"), &AmplitudeEnvelopeGenerator::set_bus_id);
    ClassDB::bind_method(D_METHOD("get_bus_id"), &AmplitudeEnvelopeGenerator::get_bus_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bus_id"), "set_bus_id", "get_bus_id");

    ClassDB::bind_method(D_METHOD("set_effect_id", "value"), &AmplitudeEnvelopeGenerator::set_effect_id);
    ClassDB::bind_method(D_METHOD("get_effect_id"), &AmplitudeEnvelopeGenerator::get_effect_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "effect_id"), "set_effect_id", "get_effect_id");

    ClassDB::bind_method(D_METHOD("set_min_frequency", "value"), &AmplitudeEnvelopeGenerator::set_min_frequency);
    ClassDB::bind_method(D_METHOD("get_min_frequency"), &AmplitudeEnvelopeGenerator::get_min_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_frequency"), "set_min_frequency", "get_min_frequency");

    ClassDB::bind_method(D_METHOD("set_max_frequency", "value"), &AmplitudeEnvelopeGenerator::set_max_frequency);
    ClassDB::bind_method(D_METHOD("get_max_frequency"), &AmplitudeEnvelopeGenerator::get_max_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_frequency"), "set_max_frequency", "get_max_frequency");

    ClassDB::bind_method(D_METHOD("set_smoothing_parameter", "value"), &AmplitudeEnvelopeGenerator::set_smoothing_parameter);
    ClassDB::bind_method(D_METHOD("get_smoothing_parameter"), &AmplitudeEnvelopeGenerator::get_smoothing_parameter);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "smoothing_parameter"), "set_smoothing_parameter", "get_smoothing_parameter");
}

AmplitudeEnvelopeGenerator::AmplitudeEnvelopeGenerator() {
    bus_id = 0;
    effect_id = 0;
    min_frequency = 20.0f;
    max_frequency = 20000.0f;
    smoothing_parameter = 0.5f;
    amplitude = 0.0f;
}

void AmplitudeEnvelopeGenerator::set_bus_id(int value) {
    bus_id = value;
}
int AmplitudeEnvelopeGenerator::get_bus_id() const {
    return bus_id;
}

void AmplitudeEnvelopeGenerator::set_effect_id(int value) {
    effect_id = value;
}
int AmplitudeEnvelopeGenerator::get_effect_id() const {
    return effect_id;
}

void AmplitudeEnvelopeGenerator::set_min_frequency(float value) {
    min_frequency = value;
}
float AmplitudeEnvelopeGenerator::get_min_frequency() const {
    return min_frequency;
}

void AmplitudeEnvelopeGenerator::set_max_frequency(float value) {
    max_frequency = value;
}
float AmplitudeEnvelopeGenerator::get_max_frequency() const {
    return max_frequency;
}

void AmplitudeEnvelopeGenerator::set_smoothing_parameter(float value) {
    smoothing_parameter = value;
}
float AmplitudeEnvelopeGenerator::get_smoothing_parameter() const {
    return smoothing_parameter;
}

void AmplitudeEnvelopeGenerator::init(Ref<SongSettings> song_settings) {
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

float AmplitudeEnvelopeGenerator::process(float delta) {
    const float MIN_DB = 60.f;
    if (!spectrum.is_valid())
        return 0.0f;    

    float magnitude = spectrum->get_magnitude_for_frequency_range(min_frequency, max_frequency, AudioEffectSpectrumAnalyzerInstance::MAGNITUDE_MAX).length();
    magnitude = Math::clamp((MIN_DB + static_cast<float>(UtilityFunctions::linear_to_db(magnitude))) / MIN_DB, 0.f, 1.f);
    amplitude = Math::lerp(amplitude, magnitude, Math::clamp(smoothing_parameter * delta, 0.0f, 1.0f));
    return amplitude;
}
