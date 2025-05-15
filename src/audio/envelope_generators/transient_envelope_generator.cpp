// transient_envelope_generator.cpp
#include "transient_envelope_generator.h"
#include "audio/song_settings.h"
#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void TransientEnvelopeGenerator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_bus_id", "value"), &TransientEnvelopeGenerator::set_bus_id);
    ClassDB::bind_method(D_METHOD("get_bus_id"), &TransientEnvelopeGenerator::get_bus_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bus_id"), "set_bus_id", "get_bus_id");

    ClassDB::bind_method(D_METHOD("set_effect_id", "value"), &TransientEnvelopeGenerator::set_effect_id);
    ClassDB::bind_method(D_METHOD("get_effect_id"), &TransientEnvelopeGenerator::get_effect_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "effect_id"), "set_effect_id", "get_effect_id");

    ClassDB::bind_method(D_METHOD("set_min_frequency", "value"), &TransientEnvelopeGenerator::set_min_frequency);
    ClassDB::bind_method(D_METHOD("get_min_frequency"), &TransientEnvelopeGenerator::get_min_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_frequency"), "set_min_frequency", "get_min_frequency");

    ClassDB::bind_method(D_METHOD("set_max_frequency", "value"), &TransientEnvelopeGenerator::set_max_frequency);
    ClassDB::bind_method(D_METHOD("get_max_frequency"), &TransientEnvelopeGenerator::get_max_frequency);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_frequency"), "set_max_frequency", "get_max_frequency");

    ClassDB::bind_method(D_METHOD("set_attack_time", "value"), &TransientEnvelopeGenerator::set_attack_time);
    ClassDB::bind_method(D_METHOD("get_attack_time"), &TransientEnvelopeGenerator::get_attack_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_time"), "set_attack_time", "get_attack_time");

    ClassDB::bind_method(D_METHOD("set_decay_time", "value"), &TransientEnvelopeGenerator::set_decay_time);
    ClassDB::bind_method(D_METHOD("get_decay_time"), &TransientEnvelopeGenerator::get_decay_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "decay_time"), "set_decay_time", "get_decay_time");

    ClassDB::bind_method(D_METHOD("set_threshold", "value"), &TransientEnvelopeGenerator::set_threshold);
    ClassDB::bind_method(D_METHOD("get_threshold"), &TransientEnvelopeGenerator::get_threshold);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshold"), "set_threshold", "get_threshold");
}

TransientEnvelopeGenerator::TransientEnvelopeGenerator()
{
    bus_id = 0;
    effect_id = 0;
    min_frequency = 20.0f;
    max_frequency = 20000.0f;
    attack_time = 0.05f; // Fast attack by default (seconds)
    decay_time = 0.5f;   // Slower decay by default (seconds)
    threshold = 0.7f;    // Trigger threshold in normalized amplitude
    envelope = 0.0f;
}

void TransientEnvelopeGenerator::set_bus_id(int value)
{
    bus_id = value;
}
int TransientEnvelopeGenerator::get_bus_id() const
{
    return bus_id;
}

void TransientEnvelopeGenerator::set_effect_id(int value)
{
    effect_id = value;
}
int TransientEnvelopeGenerator::get_effect_id() const
{
    return effect_id;
}

void TransientEnvelopeGenerator::set_min_frequency(float value)
{
    min_frequency = value;
}
float TransientEnvelopeGenerator::get_min_frequency() const
{
    return min_frequency;
}

void TransientEnvelopeGenerator::set_max_frequency(float value)
{
    max_frequency = value;
}
float TransientEnvelopeGenerator::get_max_frequency() const
{
    return max_frequency;
}

void TransientEnvelopeGenerator::set_attack_time(float value)
{
    attack_time = value;
}
float TransientEnvelopeGenerator::get_attack_time() const
{
    return attack_time;
}

void TransientEnvelopeGenerator::set_decay_time(float value)
{
    decay_time = value;
}
float TransientEnvelopeGenerator::get_decay_time() const
{
    return decay_time;
}

void TransientEnvelopeGenerator::set_threshold(float value)
{
    threshold = value;
}
float TransientEnvelopeGenerator::get_threshold() const
{
    return threshold;
}

void TransientEnvelopeGenerator::init(Ref<SongSettings> song_settings)
{
    if (!AudioServer::get_singleton())
        return;

    int bus_count = AudioServer::get_singleton()->get_bus_count();
    if (bus_id < 0 || bus_id >= bus_count)
        return;

    int effect_count = AudioServer::get_singleton()->get_bus_effect_count(bus_id);
    if (effect_id < 0 || effect_id >= effect_count)
        return;

    Ref<AudioEffect> effect = AudioServer::get_singleton()->get_bus_effect(bus_id, effect_id);
    if (effect.is_valid() && Object::cast_to<AudioEffectSpectrumAnalyzer>(*effect))
    {
        spectrum = AudioServer::get_singleton()->get_bus_effect_instance(bus_id, effect_id);
    }
}

float TransientEnvelopeGenerator::process(float delta)
{
    if (!spectrum.is_valid())
    {
        envelope = Math::lerp(envelope, 0.0f, Math::clamp(delta / decay_time, 0.0f, 1.0f));
        return envelope;
    }

    float magnitude = spectrum
                          ->get_magnitude_for_frequency_range(min_frequency, max_frequency,
                                                              AudioEffectSpectrumAnalyzerInstance::MAGNITUDE_MAX)
                          .length();

    const float MIN_DB = 60.0f;
    magnitude =
        Math::clamp((MIN_DB + static_cast<float>(UtilityFunctions::linear_to_db(magnitude))) / MIN_DB, 0.f, 1.f);

    if (magnitude >= threshold && magnitude > envelope)
    {
        envelope = Math::lerp(envelope, 1.0f, Math::clamp(delta / attack_time, 0.0f, 1.0f));
    }
    else
    {
        envelope = Math::lerp(envelope, 0.0f, Math::clamp(delta / decay_time, 0.0f, 1.0f));
    }

    envelope = Math::clamp(envelope, 0.0f, 1.0f);
    return envelope;
}
