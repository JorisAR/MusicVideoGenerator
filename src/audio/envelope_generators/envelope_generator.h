#ifndef ENVELOPE_GENERATOR_H
#define ENVELOPE_GENERATOR_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "audio/song_settings.h"

using namespace godot;

enum class EnvelopePhase {
  ENVELOPE_IDLE,
  ENVELOPE_ATTACK,
  ENVELOPE_RELEASE,
};

class EnvelopeGenerator : public Resource
{
    GDCLASS(EnvelopeGenerator, Resource);

  public:
    EnvelopeGenerator();
    virtual ~EnvelopeGenerator() = default;

    // The process function takes delta (time elapsed in seconds) and an optional audio input.
    // For generators that do not use audio input, the parameter is ignored.
    virtual float process(float delta) = 0;

    virtual void init(Ref<SongSettings> song_settings) = 0;


  protected:
    static void _bind_methods();

    Ref<SongSettings> _song_settings;


};

#endif // ENVELOPE_GENERATOR_H
