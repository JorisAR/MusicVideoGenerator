#include "animation_sequence_move.h"

using namespace godot;

void AnimationSequenceMove::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_start_position", "p"), &AnimationSequenceMove::set_start_position);
    ClassDB::bind_method(D_METHOD("set_end_position", "p"), &AnimationSequenceMove::set_end_position);
    ClassDB::bind_method(D_METHOD("get_start_position"), &AnimationSequenceMove::get_start_position);
    ClassDB::bind_method(D_METHOD("get_end_position"), &AnimationSequenceMove::get_end_position);
    ClassDB::bind_method(D_METHOD("set_look_at_position", "p"), &AnimationSequenceMove::set_look_at_position);
    ClassDB::bind_method(D_METHOD("get_look_at_position"), &AnimationSequenceMove::get_look_at_position);
    ClassDB::bind_method(D_METHOD("set_look_at_enabled", "enabled"), &AnimationSequenceMove::set_look_at_enabled);
    ClassDB::bind_method(D_METHOD("get_look_at_enabled"), &AnimationSequenceMove::get_look_at_enabled);

    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "start_position"), "set_start_position", "get_start_position");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "end_position"), "set_end_position", "get_end_position");

    ADD_GROUP("Look At", "look_at_");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "look_at_enabled"), "set_look_at_enabled", "get_look_at_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "look_at_position"), "set_look_at_position", "get_look_at_position");
}

AnimationSequenceMove::AnimationSequenceMove() {
    start_position = Vector3();
    end_position = Vector3();
}

void AnimationSequenceMove::init() {

}

void AnimationSequenceMove::process(Node3D* rig, float delta, float progress) {
    rig->set_position(start_position.lerp(end_position, progress));
    if (look_at_enabled) {
        rig->look_at(look_at_position);
    }
    else { //look forward
        Vector3 towards = (end_position - start_position) * 2.0f;
        Vector3 forward = towards.normalized();

        Vector3 up = Vector3(0, 1, 0);

        if (fabs(forward.dot(up)) > 0.99f) {
            up = Vector3(0, 0, 1);
        }

        rig->look_at(rig->get_position() + towards, up);

    }
}

void AnimationSequenceMove::set_start_position(const Vector3 &p) {
    start_position = p;
}

void AnimationSequenceMove::set_end_position(const Vector3 &p) {
    end_position = p;
}

Vector3 AnimationSequenceMove::get_start_position() const {
    return start_position;
}

Vector3 AnimationSequenceMove::get_end_position() const {
    return end_position;
}

void AnimationSequenceMove::set_look_at_position(const Vector3 &p) {
    look_at_position = p;
}

Vector3 AnimationSequenceMove::get_look_at_position() const {
    return look_at_position;
}

void AnimationSequenceMove::set_look_at_enabled(bool enabled) {
    look_at_enabled = enabled;
}

bool AnimationSequenceMove::get_look_at_enabled() const {
    return look_at_enabled;
}
