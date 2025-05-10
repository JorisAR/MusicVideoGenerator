#ifndef SONG_SETTINGS_H
#define SONG_SETTINGS_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

class SongSettings : public Resource {
    GDCLASS(SongSettings, Resource);

public:
    SongSettings();
    ~SongSettings();
    
    void set_bpm(float p_bpm);
    float get_bpm() const;

    void set_time_signature_numerator(int p_numerator);
    int get_time_signature_numerator() const;

    void set_time_signature_denominator(int p_denominator);
    int get_time_signature_denominator() const;

    void set_song_title(const String &p_title);
    String get_song_title() const;

    void set_time_offset(float p_offset);
    float get_time_offset() const;

protected:
    static void _bind_methods();

private:
    int time_signature_numerator;
    int time_signature_denominator;
    float bpm;
    String song_title;
    float time_offset;
};

#endif // SONG_SETTINGS_H
