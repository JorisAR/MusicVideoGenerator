#ifndef ANIMATION_SEQUENCE_MOVE_H
#define ANIMATION_SEQUENCE_MOVE_H

#include "animation_sequence.h"
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class AnimationSequenceMove : public AnimationSequence {
    GDCLASS(AnimationSequenceMove, AnimationSequence);

public:
    AnimationSequenceMove();
    ~AnimationSequenceMove() override = default;

    void init() override;

    void process(Node3D* rig, float delta, float progress) override;

    // Setters/getters for start and end positions.
    void set_start_position(const Vector3 &p);
    void set_end_position(const Vector3 &p);
    Vector3 get_start_position() const;
    Vector3 get_end_position() const;

    // Setters/getters for look_at_position and look_at_enabled.
    void set_look_at_position(const Vector3 &p);
    Vector3 get_look_at_position() const;
    void set_look_at_enabled(bool enabled);
    bool get_look_at_enabled() const;

protected:
    static void _bind_methods();

private:
    Vector3 start_position;
    Vector3 end_position;
    Vector3 look_at_position;
    bool look_at_enabled = false;
};

#endif // ANIMATION_SEQUENCE_MOVE_H
