#ifndef ANIMATION_SEQUENCE_H
#define ANIMATION_SEQUENCE_H

#include "audio/song_settings.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

class AnimationSequence : public Resource
{
    GDCLASS(AnimationSequence, Resource);

  public:
    AnimationSequence();
    virtual ~AnimationSequence() = default;

    virtual void process(Node3D* rig, float delta, float progress) = 0;

    virtual void init() = 0;

    void set_duration_bars(float bars);
    float get_duration_bars() const;

  protected:
    static void _bind_methods(); 
    float duration_bars = 4;
};

#endif // ANIMATION_SEQUENCE_H
