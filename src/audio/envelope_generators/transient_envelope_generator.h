// transient_envelope_generator.h
#ifndef TRANSIENT_ENVELOPE_GENERATOR_H
#define TRANSIENT_ENVELOPE_GENERATOR_H

#include "envelope_generator.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer_instance.hpp>

class TransientEnvelopeGenerator : public EnvelopeGenerator {
    GDCLASS(TransientEnvelopeGenerator, EnvelopeGenerator);

public:
    TransientEnvelopeGenerator();
    virtual ~TransientEnvelopeGenerator() {}

    // Initialization using song settings to grab the spectrum analyzer instance
    virtual void init(Ref<SongSettings> song_settings) override;
    // Process function outputs the transient envelope value based on audio input.
    virtual float process(float delta) override;

    // Getters and setters for audio bus and effect IDs.
    void set_bus_id(int value);
    int get_bus_id() const;

    void set_effect_id(int value);
    int get_effect_id() const;

    // Frequency range for processing.
    void set_min_frequency(float value);
    float get_min_frequency() const;

    void set_max_frequency(float value);
    float get_max_frequency() const;

    // Attack and decay time (in seconds)
    void set_attack_time(float value);
    float get_attack_time() const;

    void set_decay_time(float value);
    float get_decay_time() const;

    // Threshold for triggering the transient (normalized value from 0 to 1)
    void set_threshold(float value);
    float get_threshold() const;

protected:
    static void _bind_methods();

    int bus_id;
    int effect_id;
    float min_frequency;
    float max_frequency;
    float attack_time;         // Time constant for the attack phase (fast rise)
    float decay_time;          // Time constant for the decay phase (gradual drop)
    float threshold;           // Normalized threshold to trigger the transient

    float envelope;  // The current value of the transient envelope (0 to 1)
    Ref<AudioEffectSpectrumAnalyzerInstance> spectrum;
};

#endif // TRANSIENT_ENVELOPE_GENERATOR_H
