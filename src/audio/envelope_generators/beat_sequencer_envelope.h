#ifndef BEAT_SEQUENCER_ENVELOPE_H
#define BEAT_SEQUENCER_ENVELOPE_H

#include "envelope_generator.h"
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class BeatSequencerEnvelope : public EnvelopeGenerator
{
    GDCLASS(BeatSequencerEnvelope, EnvelopeGenerator);

  public:
    BeatSequencerEnvelope();
    virtual ~BeatSequencerEnvelope()
    {
    }

    // Process the envelope. Ignores audio_input.
    virtual void init(Ref<SongSettings> song_settings) override;
    virtual float process(float delta) override;

    // --- Beat Sequence ---
    // An array of booleans; each element represents a beat in the measure.
    void set_beat_sequence(const TypedArray<bool> p_sequence);
    TypedArray<bool> get_beat_sequence() const;

    // --- Envelope Parameters ---
    void set_attack_time(float p_attack_time);
    float get_attack_time() const;

    void set_release_time(float p_release_time);
    float get_release_time() const;

  protected:
    static void _bind_methods();
    TypedArray<bool> beat_sequence; // For example: [true, false, true, false] for a 4/4 measure.
    float attack_time;   // Duration (seconds) for the envelope to reach peak.
    float release_time;  // Duration (seconds) for the envelope to decay.
    float envelope;      // Current envelope value (range 0 to 1).

    float time_since_last_beat;
    int current_beat_index;

    EnvelopePhase envelope_phase = EnvelopePhase::ENVELOPE_IDLE;
    float phase_timer = 0.0f;
};

#endif // BEAT_SEQUENCER_ENVELOPE_H
