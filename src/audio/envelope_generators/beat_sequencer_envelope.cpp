#include "beat_sequencer_envelope.h"

using namespace godot;

BeatSequencerEnvelope::BeatSequencerEnvelope() {
    attack_time = 0.05f;
    release_time = 0.2f;

    beat_sequence = Array();
    for (int i = 0; i < 4; i++) {
        beat_sequence.append(false);
    }
}



void BeatSequencerEnvelope::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_beat_sequence", "sequence"), &BeatSequencerEnvelope::set_beat_sequence);
    ClassDB::bind_method(D_METHOD("get_beat_sequence"), &BeatSequencerEnvelope::get_beat_sequence);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "sequence"), "set_beat_sequence", "get_beat_sequence");

    ClassDB::bind_method(D_METHOD("set_attack_time", "attack_time"), &BeatSequencerEnvelope::set_attack_time);
    ClassDB::bind_method(D_METHOD("get_attack_time"), &BeatSequencerEnvelope::get_attack_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "attack_time"), "set_attack_time", "get_attack_time");

    ClassDB::bind_method(D_METHOD("set_release_time", "release_time"), &BeatSequencerEnvelope::set_release_time);
    ClassDB::bind_method(D_METHOD("get_release_time"), &BeatSequencerEnvelope::get_release_time);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "release_time"), "set_release_time", "get_release_time");
}

void BeatSequencerEnvelope::init(Ref<SongSettings> song_settings)
{
    _song_settings = song_settings;
    envelope = 0.0f;
    time_since_last_beat = 0.0f;
    current_beat_index = -1; 
    envelope_phase = EnvelopePhase::ENVELOPE_IDLE;
    phase_timer = 0.0f;
}

// Process the envelope per frame.
float BeatSequencerEnvelope::process(float delta) {
    // Calculate beat interval from BPM (assuming BPM is for quarter notes).
    float beat_interval = 60.0f / _song_settings->get_bpm();
    time_since_last_beat += delta;

    if (time_since_last_beat >= beat_interval || current_beat_index < 0) {
        time_since_last_beat = 0.0f;
        current_beat_index = (current_beat_index + 1) % beat_sequence.size();
        // Only trigger the envelope if this beat is active.
        if (bool(beat_sequence[current_beat_index])) {
            // Reset envelope state on trigger.
            envelope = 0.0f;
            phase_timer = 0.0f;
            envelope_phase = EnvelopePhase::ENVELOPE_ATTACK;
        }
    }

    // Process based on the current envelope phase.
    switch (envelope_phase) {
        case EnvelopePhase::ENVELOPE_ATTACK: {
            phase_timer += delta;
            // Ramp up linearly.
            envelope = phase_timer / Math::max(attack_time, 1e-6f);
            // Clamping at 1.0 and switching to release phase when done.
            if (envelope >= 1.0f) {
                envelope = 1.0f;
                phase_timer = 0.0f;
                envelope_phase = EnvelopePhase::ENVELOPE_RELEASE;
            }
            break;
        }
        case EnvelopePhase::ENVELOPE_RELEASE: {
            phase_timer += delta;
            // Ramp down linearly.
            envelope = 1.0f - (phase_timer / Math::max(release_time, 1e-6f));
            if (envelope <= 0.0f) {
                envelope = 0.0f;
                envelope_phase = EnvelopePhase::ENVELOPE_IDLE;
            }
            break;
        }
        case EnvelopePhase::ENVELOPE_IDLE:
        default:
            // Maintain silence when idle.
            envelope = 0.0f;
            break;
    }

    return envelope;
}

// The rest of the setters/getters remain unchanged.



void BeatSequencerEnvelope::set_beat_sequence(const TypedArray<bool> p_sequence)
{
    beat_sequence = p_sequence;
}

TypedArray<bool> BeatSequencerEnvelope::get_beat_sequence() const {
    return beat_sequence;
}

void BeatSequencerEnvelope::set_attack_time(float p_attack_time) {
    attack_time = p_attack_time;
}

float BeatSequencerEnvelope::get_attack_time() const {
    return attack_time;
}

void BeatSequencerEnvelope::set_release_time(float p_release_time) {
    release_time = p_release_time;
}

float BeatSequencerEnvelope::get_release_time() const {
    return release_time;
}
