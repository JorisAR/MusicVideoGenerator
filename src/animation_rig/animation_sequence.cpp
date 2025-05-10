#include "animation_sequence.h"

using namespace godot;

void AnimationSequence::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_duration_bars", "bars"), &AnimationSequence::set_duration_bars);
    ClassDB::bind_method(D_METHOD("get_duration_bars"), &AnimationSequence::get_duration_bars);


    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration_bars"), "set_duration_bars", "get_duration_bars");

}

AnimationSequence::AnimationSequence() {
}

void AnimationSequence::init() {
    duration_bars = 4;
}

void AnimationSequence::set_duration_bars(float bars) {
    duration_bars = bars;
}

float AnimationSequence::get_duration_bars() const {
    return duration_bars;
}
