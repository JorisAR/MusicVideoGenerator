#ifndef PATH_TRACING_CAMERA_H
#define PATH_TRACING_CAMERA_H

#include "gdcs/include/gdcs.h"
#include "audio/music_manager.h"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/texture2drd.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/rd_texture_format.hpp>
#include <godot_cpp/classes/rd_texture_view.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/projection.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/display_server.hpp>

using namespace godot;

class RayMarchingCamera : public Node3D
{
    GDCLASS(RayMarchingCamera, Node3D);

  public:

    struct RenderParameters // match the struct on the gpu
    {
        Vector4 backgroundColor;
        int width;
        int height;
        float fov;
        float coneResolutionScale;

        PackedByteArray to_packed_byte_array()
        {
            PackedByteArray byte_array;
            byte_array.resize(sizeof(RenderParameters));
            std::memcpy(byte_array.ptrw(), this, sizeof(RenderParameters));
            return byte_array;
        }
    };

    struct ConeMarchingParameters // match the struct on the gpu
    {
        int width;
        int height;
        float fov;
        float pixelConeAngle;

        PackedByteArray to_packed_byte_array()
        {
            PackedByteArray byte_array;
            byte_array.resize(sizeof(ConeMarchingParameters));
            std::memcpy(byte_array.ptrw(), this, sizeof(ConeMarchingParameters));
            return byte_array;
        }
    };

    struct CameraParameters // match the struct on the gpu
    {
        float vp[16];
        float ivp[16];
        Vector4 cameraPosition;
        int frame_index;
        float nearPlane;
        float farPlane;
        float time;

        PackedByteArray to_packed_byte_array()
        {
            PackedByteArray byte_array;
            byte_array.resize(sizeof(CameraParameters));
            std::memcpy(byte_array.ptrw(), this, sizeof(CameraParameters));
            return byte_array;
        }
    };

    struct MusicData
    {
        Vector4 current_magnitude_data;
        Vector4 cumulative_magnitude_data;
        float spectrum[MusicManager::MAX_SPECTRUM_SIZE];
        int spectrum_count;

        PackedByteArray to_packed_byte_array()
        {
            PackedByteArray byte_array;
            byte_array.resize(sizeof(MusicData));
            std::memcpy(byte_array.ptrw(), this, sizeof(MusicData));
            return byte_array;
        }
    };

  protected:
    static void _bind_methods();

  public:
    void _notification(int what);

    float get_fov() const;
    void set_fov(float value);

    float get_near_plane() const;
    void set_near_plane(float value);

    float get_far_plane() const;
    void set_far_plane(float value);

    Color get_background_color() const;
    void set_background_color(const Color &value);

    int get_cone_resolution_scale() const;
    void set_cone_resolution_scale(int value);

    bool is_cone_marching_enabled() const;
    void set_cone_marching_enabled(bool value);

    // int get_num_bounces() const;
    // void set_num_bounces(int value);

    TextureRect *get_output_texture() const;
    void set_output_texture(TextureRect *value);

    MusicManager *get_music_manager() const;
    void set_music_manager(MusicManager *value);

    
    int get_scene_id() const;
    void set_scene_id(int value);


  private:
    void init();
    void clear_compute_shader();
    void render();

    float fov = 90.0f;
    int scene_id = 0;
    float near_plane = 0.5f;
    float far_plane = 1000.0f;
    Color background_color = Color(0.0, 0.0, 0.0, 1.0);
    int cone_resolution_scale = 10;
    bool cone_marching_enabled = true;

    ComputeShader *ray_marching_shader = nullptr;
    ComputeShader *cone_marching_shader = nullptr;
    TextureRect *output_texture_rect = nullptr;
    MusicManager *music_manager = nullptr;
    Ref<Image> output_image;
    Ref<Image> depth_image;
    Ref<Image> cone_image;
    // Ref<ImageTexture> output_texture;
    Ref<Texture2DRD> output_texture;

    ConeMarchingParameters cone_marching_parameters;
    RenderParameters render_parameters;
    CameraParameters camera_parameters;
    MusicData music_data;
    Projection projection_matrix;   


    // BUFFER IDs
    RID output_texture_rid;
    RID depth_texture_rid;
    RID cone_texture_rid;
    RID cone_marching_parameters_rid;
    RID render_parameters_rid;
    RID camera_parameters_rid;
    RID music_data_rid;
    RenderingDevice *_rd;
};

#endif // PATH_TRACING_CAMERA_H