#ifndef AMPLITUDE_ENVELOPE_GENERATOR_H
#define AMPLITUDE_ENVELOPE_GENERATOR_H

#include "envelope_generator.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/classes/audio_effect_spectrum_analyzer_instance.hpp>

class AmplitudeEnvelopeGenerator : public EnvelopeGenerator
{
    GDCLASS(AmplitudeEnvelopeGenerator, EnvelopeGenerator);

  public:
    AmplitudeEnvelopeGenerator();
    virtual ~AmplitudeEnvelopeGenerator()
    {
    }

    // Process function uses the audio input to compute a transient trigger.
    virtual void init(Ref<SongSettings> song_settings) override;
    virtual float process(float delta) override;

    // Getters and setters
    void set_bus_id(int value);
    int get_bus_id() const;

    void set_effect_id(int value);
    int get_effect_id() const;

    void set_min_frequency(float value);
    float get_min_frequency() const;

    void set_max_frequency(float value);
    float get_max_frequency() const;

    void set_smoothing_parameter(float value);
    float get_smoothing_parameter() const;

  protected:
    static void _bind_methods();
    int bus_id;
    int effect_id;
    float min_frequency;
    float max_frequency;
    float smoothing_parameter;

    float amplitude; //update and return this in process
    Ref<AudioEffectSpectrumAnalyzerInstance> spectrum; //get this in init
};

#endif // AMPLITUDE_ENVELOPE_GENERATOR_H
