#ifndef DUAL_ENVELOPE_TRANSIENT_GENERATOR_H
#define DUAL_ENVELOPE_TRANSIENT_GENERATOR_H

#include "envelope_generator.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>

class DualEnvelopeTransientGenerator : public EnvelopeGenerator
{
    GDCLASS(DualEnvelopeTransientGenerator, EnvelopeGenerator);

  public:
    DualEnvelopeTransientGenerator();
    virtual ~DualEnvelopeTransientGenerator()
    {
    }

    // Process function uses the audio input to compute a transient trigger.
    virtual void init(Ref<SongSettings> song_settings) override;
    virtual float process(float delta) override;

    // Accessor methods for adjustability.
    void set_fast_attack_alpha(float p_val);
    float get_fast_attack_alpha() const;

    void set_fast_release_alpha(float p_val);
    float get_fast_release_alpha() const;

    void set_slow_alpha(float p_val);
    float get_slow_alpha() const;

    void set_threshold_delta(float p_val);
    float get_threshold_delta() const;

    void set_transient_decay_rate(float p_val);
    float get_transient_decay_rate() const;

  protected:
    static void _bind_methods();
    float fast_env;
    float slow_env;
    float transient_value;

    float fast_attack_alpha;
    float fast_release_alpha;
    float slow_alpha;
    float threshold_delta;
    float transient_decay_rate;
};

#endif 
