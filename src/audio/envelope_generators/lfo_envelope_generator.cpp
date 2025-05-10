#include "lfo_envelope_generator.h"
#include <math.h>

using namespace godot;

void LFOEnvelopeGenerator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_lfo_mode", "mode"), &LFOEnvelopeGenerator::set_lfo_mode);
    ClassDB::bind_method(D_METHOD("get_lfo_mode"), &LFOEnvelopeGenerator::get_lfo_mode);

    BIND_ENUM_CONSTANT(LFO_SINE);
    BIND_ENUM_CONSTANT(LFO_SQUARE);
    BIND_ENUM_CONSTANT(LFO_TRIANGLE);
    BIND_ENUM_CONSTANT(LFO_SAW);
    BIND_ENUM_CONSTANT(LFO_NOISE);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lfo_mode", PROPERTY_HINT_ENUM, "Sine,Square,Triangle,Saw,Noise"), "set_lfo_mode", "get_lfo_mode");

    ClassDB::bind_method(D_METHOD("set_min_value", "value"), &LFOEnvelopeGenerator::set_min_value);
    ClassDB::bind_method(D_METHOD("get_min_value"), &LFOEnvelopeGenerator::get_min_value);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_value"), "set_min_value", "get_min_value");

    ClassDB::bind_method(D_METHOD("set_max_value", "value"), &LFOEnvelopeGenerator::set_max_value);
    ClassDB::bind_method(D_METHOD("get_max_value"), &LFOEnvelopeGenerator::get_max_value);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_value"), "set_max_value", "get_max_value");

    ClassDB::bind_method(D_METHOD("set_duration_bars", "value"), &LFOEnvelopeGenerator::set_duration_bars);
    ClassDB::bind_method(D_METHOD("get_duration_bars"), &LFOEnvelopeGenerator::get_duration_bars);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration_bars"), "set_duration_bars", "get_duration_bars");

    ClassDB::bind_method(D_METHOD("set_phase_bars", "value"), &LFOEnvelopeGenerator::set_phase_bars);
    ClassDB::bind_method(D_METHOD("get_phase_bars"), &LFOEnvelopeGenerator::get_phase_bars);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "phase_bars"), "set_phase_bars", "get_phase_bars");
}

//every 1 unit of time is 1 full cycle of the LFO
float LFOEnvelopeGenerator::evalute_lfo(float time)
{
    switch (lfo_mode)
    {
        case LFOMode::LFO_SINE:
            return sin(6.28318530718f * time);
        case LFOMode::LFO_SQUARE:

            return (time < 0.5f) ? 1.0f : -1.0f;
        case LFOMode::LFO_TRIANGLE:
            return 2.0f * abs(2.0f * (time - floor(time + 0.5f))) - 1.0f;
        case LFOMode::LFO_SAW:
            return 2.0f * (time - floor(time + 0.5f));
        case LFOMode::LFO_NOISE:
            return rng->randf_range(-1.0f, 1.0f);
        default:
            return 0.0f;
    }
}

LFOEnvelopeGenerator::LFOEnvelopeGenerator()
{
    lfo_mode = LFOMode::LFO_SINE;
    duration_bars = 4.0f;
    min_value = 0.0f;
    max_value = 1.0f;
    time_elapsed = 0.0f;
    rng.instantiate();
}

void LFOEnvelopeGenerator::init(Ref<SongSettings> song_settings)
{
    _song_settings = song_settings;
    time_elapsed = 0.0f;
}

// Process the envelope per frame.
float LFOEnvelopeGenerator::process(float delta) {
    float bpm = _song_settings->get_bpm();
    float beats_per_bar = _song_settings->get_time_signature_numerator();
    float seconds_per_bar = (60.0f / bpm) * beats_per_bar * (_song_settings->get_time_signature_denominator() / 4.0f);

    float normalized_time = (time_elapsed / seconds_per_bar + phase_bars) / duration_bars;
        
    float int_part;
    float lfo_value = evalute_lfo(std::modf(normalized_time, &int_part));
    time_elapsed += delta;
    return (0.5f * lfo_value + 0.5f) * (max_value - min_value) + min_value;
}
