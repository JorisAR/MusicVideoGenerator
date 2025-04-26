#include "song_settings.h"

using namespace godot;

void SongSettings::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_bpm", "p_bpm"), &SongSettings::set_bpm);
    ClassDB::bind_method(D_METHOD("get_bpm"), &SongSettings::get_bpm);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bpm"), "set_bpm", "get_bpm");

    ClassDB::bind_method(D_METHOD("set_time_signature_numerator", "p_numerator"), &SongSettings::set_time_signature_numerator);
    ClassDB::bind_method(D_METHOD("get_time_signature_numerator"), &SongSettings::get_time_signature_numerator);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "time_signature_numerator"), "set_time_signature_numerator", "get_time_signature_numerator");

    ClassDB::bind_method(D_METHOD("set_time_signature_denominator", "p_denominator"), &SongSettings::set_time_signature_denominator);
    ClassDB::bind_method(D_METHOD("get_time_signature_denominator"), &SongSettings::get_time_signature_denominator);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "time_signature_denominator"), "set_time_signature_denominator", "get_time_signature_denominator");

    ClassDB::bind_method(D_METHOD("set_song_title", "p_title"), &SongSettings::set_song_title);
    ClassDB::bind_method(D_METHOD("get_song_title"), &SongSettings::get_song_title);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "song_title"), "set_song_title", "get_song_title");

    ClassDB::bind_method(D_METHOD("set_time_offset", "p_offset"), &SongSettings::set_time_offset);
    ClassDB::bind_method(D_METHOD("get_time_offset"), &SongSettings::get_time_offset);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_offset"), "set_time_offset", "get_time_offset");
}

SongSettings::SongSettings() :
    time_signature_numerator(4),
    time_signature_denominator(4),
    bpm(120.0f),
    song_title("Untitled"),
    time_offset(0.0f)
{
}

SongSettings::~SongSettings() {
}

void SongSettings::set_bpm(float p_bpm) {
    bpm = p_bpm;
}

float SongSettings::get_bpm() const {
    return bpm;
}

void SongSettings::set_time_signature_numerator(int p_numerator) {
    time_signature_numerator = p_numerator;
}

int SongSettings::get_time_signature_numerator() const {
    return time_signature_numerator;
}

void SongSettings::set_time_signature_denominator(int p_denominator) {
    time_signature_denominator = p_denominator;
}

int SongSettings::get_time_signature_denominator() const {
    return time_signature_denominator;
}

void SongSettings::set_song_title(const String &p_title) {
    song_title = p_title;
}

String SongSettings::get_song_title() const {
    return song_title;
}

void SongSettings::set_time_offset(float p_offset) {
    time_offset = p_offset;
}

float SongSettings::get_time_offset() const {
    return time_offset;
}
