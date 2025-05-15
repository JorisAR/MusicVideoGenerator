// dual_envelope_transient_generator.h
#ifndef DUAL_ENVELOPE_TRANSIENT_GENERATOR_H
#define DUAL_ENVELOPE_TRANSIENT_GENERATOR_H

#include "envelope_generator.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer_instance.hpp>

class DualEnvelopeTransientGenerator : public EnvelopeGenerator {
    GDCLASS(DualEnvelopeTransientGenerator, EnvelopeGenerator);

public:
    DualEnvelopeTransientGenerator();
    virtual ~DualEnvelopeTransientGenerator() {}

    // Initialization: Grab the spectrum analyzer instance from SongSettings.
    virtual void init(Ref<SongSettings> song_settings) override;
    // Process the current audio state and return the transient envelope value.
    virtual float process(float delta) override;

    // Bus and effect configuration.
    void set_bus_id(int value);
    int get_bus_id() const;

    void set_effect_id(int value);
    int get_effect_id() const;

    // Frequency range for processing.
    void set_min_frequency(float value);
    float get_min_frequency() const;

    void set_max_frequency(float value);
    float get_max_frequency() const;

    // Fast envelope parameters (in seconds).
    void set_fast_attack(float value);
    float get_fast_attack() const;

    void set_fast_decay(float value);
    float get_fast_decay() const;

    // Slow envelope parameters (in seconds).
    void set_slow_attack(float value);
    float get_slow_attack() const;

    void set_slow_decay(float value);
    float get_slow_decay() const;

    // Detection threshold:
    // The difference (fast envelope – slow envelope) must exceed this value to trigger a transient.
    void set_detection_threshold(float value);
    float get_detection_threshold() const;

    // Output transient envelope parameters.
    // These define how quickly the transient output rises and falls.
    void set_output_attack(float value);
    float get_output_attack() const;

    void set_output_decay(float value);
    float get_output_decay() const;

protected:
    static void _bind_methods();

    int bus_id;
    int effect_id;
    float min_frequency;
    float max_frequency;

    // Fast envelope timing.
    float fast_attack;
    float fast_decay;

    // Slow envelope timing.
    float slow_attack;
    float slow_decay;

    // The difference between envelopes must exceed this threshold.
    float detection_threshold;

    // Output transient envelope timing.
    float output_attack;
    float output_decay;

    // Internal state variables.
    float envelope_fast;
    float envelope_slow;
    float transient;

    Ref<AudioEffectSpectrumAnalyzerInstance> spectrum;
};

#endif // DUAL_ENVELOPE_TRANSIENT_GENERATOR_H
