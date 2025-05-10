#ifndef LFO_ENVELOPE_GENERATOR_H
#define LFO_ENVELOPE_GENERATOR_H

#include "envelope_generator.h"
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/typed_array.hpp>

using namespace godot;

class LFOEnvelopeGenerator : public EnvelopeGenerator
{
    GDCLASS(LFOEnvelopeGenerator, EnvelopeGenerator);

  public:
    enum LFOMode
    {
        LFO_SINE,
        LFO_SQUARE,
        LFO_TRIANGLE,
        LFO_SAW,
        LFO_NOISE
    };

    LFOEnvelopeGenerator();
    virtual ~LFOEnvelopeGenerator()
    {
    }

    LFOMode get_lfo_mode() const
    {
        return lfo_mode;
    }
    void set_lfo_mode(LFOMode mode)
    {
        lfo_mode = mode;
    }
    float get_duration_bars() const
    {
        return duration_bars;
    }
    void set_duration_bars(float bars)
    {
        duration_bars = bars;
    }
    float get_phase_bars() const
    {
        return phase_bars;
    }
    void set_phase_bars(float bars)
    {
        phase_bars = bars;
    }
    float get_min_value() const
    {
        return min_value;
    }
    void set_min_value(float value)
    {
        min_value = value;
    }
    float get_max_value() const
    {
        return max_value;
    }
    void set_max_value(float value)
    {
        max_value = value;
    }

    // Process the envelope. Ignores audio_input.
    virtual void init(Ref<SongSettings> song_settings) override;
    virtual float process(float delta) override;

  protected:
    static void _bind_methods();

    LFOMode lfo_mode = LFO_SINE;
    float duration_bars = 4;
    float phase_bars = 0;
    float min_value = 0.0f;
    float max_value = 1.0f;

    float time_elapsed;

  private:
    float evalute_lfo(float time);
    Ref<RandomNumberGenerator> rng;
};

VARIANT_ENUM_CAST(LFOEnvelopeGenerator::LFOMode);

#endif // LFO_ENVELOPE_GENERATOR_H
