#include "ray_marching_camera.h"
#include "utility/utils.h"

void RayMarchingCamera::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_fov"), &RayMarchingCamera::get_fov);
    ClassDB::bind_method(D_METHOD("set_fov", "value"), &RayMarchingCamera::set_fov);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fov"), "set_fov", "get_fov");

    ClassDB::bind_method(D_METHOD("get_scene_id"), &RayMarchingCamera::get_scene_id);
    ClassDB::bind_method(D_METHOD("set_scene_id", "value"), &RayMarchingCamera::set_scene_id);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "scene_id"), "set_scene_id", "get_scene_id");

    ClassDB::bind_method(D_METHOD("get_output_texture"), &RayMarchingCamera::get_output_texture);
    ClassDB::bind_method(D_METHOD("set_output_texture", "value"), &RayMarchingCamera::set_output_texture);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "output_texture", PROPERTY_HINT_NODE_TYPE, "TextureRect"),
                 "set_output_texture", "get_output_texture");

    ClassDB::bind_method(D_METHOD("get_music_manager"), &RayMarchingCamera::get_music_manager);
    ClassDB::bind_method(D_METHOD("set_music_manager", "value"), &RayMarchingCamera::set_music_manager);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "music_manager", PROPERTY_HINT_NODE_TYPE, "MusicManager"),
                 "set_music_manager", "get_music_manager");

    ClassDB::bind_method(D_METHOD("get_near_plane"), &RayMarchingCamera::get_near_plane);
    ClassDB::bind_method(D_METHOD("set_near_plane", "value"), &RayMarchingCamera::set_near_plane);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "near_plane"), "set_near_plane", "get_near_plane");

    ClassDB::bind_method(D_METHOD("get_far_plane"), &RayMarchingCamera::get_far_plane);
    ClassDB::bind_method(D_METHOD("set_far_plane", "value"), &RayMarchingCamera::set_far_plane);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "far_plane"), "set_far_plane", "get_far_plane");

    ClassDB::bind_method(D_METHOD("get_background_color"), &RayMarchingCamera::get_background_color);
    ClassDB::bind_method(D_METHOD("set_background_color", "value"), &RayMarchingCamera::set_background_color);
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "background_color"), "set_background_color", "get_background_color");

    ClassDB::bind_method(D_METHOD("get_cone_resolution_scale"), &RayMarchingCamera::get_cone_resolution_scale);
    ClassDB::bind_method(D_METHOD("set_cone_resolution_scale", "value"), &RayMarchingCamera::set_cone_resolution_scale);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "cone_resolution_scale"), "set_cone_resolution_scale",
                 "get_cone_resolution_scale");

    ClassDB::bind_method(D_METHOD("is_cone_marching_enabled"), &RayMarchingCamera::is_cone_marching_enabled);
    ClassDB::bind_method(D_METHOD("set_cone_marching_enabled", "value"), &RayMarchingCamera::set_cone_marching_enabled);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "cone_marching_enabled"), "set_cone_marching_enabled",
                 "is_cone_marching_enabled");
}

void RayMarchingCamera::_notification(int p_what)
{
    if (godot::Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    switch (p_what)
    {
    case NOTIFICATION_ENTER_TREE: {
        set_process_internal(true);

        break;
    }
    case NOTIFICATION_EXIT_TREE: {
        set_process_internal(false);

        break;
    }
    case NOTIFICATION_READY: {
        init();
    }
    case NOTIFICATION_INTERNAL_PROCESS: {
        render();
        break;
    }
    }
}

float RayMarchingCamera::get_fov() const
{
    return fov;
}

void RayMarchingCamera::set_fov(float value)
{
    fov = value;
}

float RayMarchingCamera::get_near_plane() const
{
    return near_plane;
}

void RayMarchingCamera::set_near_plane(float value)
{
    near_plane = value;
}

float RayMarchingCamera::get_far_plane() const
{
    return far_plane;
}

void RayMarchingCamera::set_far_plane(float value)
{
    far_plane = value;
}

Color RayMarchingCamera::get_background_color() const
{
    return background_color;
}

void RayMarchingCamera::set_background_color(const Color &value)
{
    background_color = value;
}

int RayMarchingCamera::get_cone_resolution_scale() const
{
    return cone_resolution_scale;
}

void RayMarchingCamera::set_cone_resolution_scale(int value)
{
    cone_resolution_scale = value;
}

bool RayMarchingCamera::is_cone_marching_enabled() const
{
    return cone_marching_enabled;
}

void RayMarchingCamera::set_cone_marching_enabled(bool value)
{
    cone_marching_enabled = value;
}

// int RayMarchingCamera::get_num_bounces() const
// {
//     return num_bounces;
// }

// void RayMarchingCamera::set_num_bounces(int value)
// {
//     num_bounces = value;
// }

TextureRect *RayMarchingCamera::get_output_texture() const
{
    return output_texture_rect;
}

void RayMarchingCamera::set_music_manager(MusicManager *value)
{
    music_manager = value;
}

int RayMarchingCamera::get_scene_id() const
{
    return scene_id;
}

void RayMarchingCamera::set_scene_id(int value)
{
    scene_id = value;
}

void RayMarchingCamera::set_output_texture(TextureRect *value)
{
    output_texture_rect = value;
}

MusicManager *RayMarchingCamera::get_music_manager() const
{
    return music_manager;
}

void RayMarchingCamera::init()
{
    // we want to use one RD for all shaders relevant to the camera.
    // _rd = RenderingServer::get_singleton()->create_local_rendering_device();

    _rd = RenderingServer::get_singleton()->get_rendering_device();

    // get resolution
    Vector2i resolution = DisplayServer::get_singleton()->window_get_size();

    projection_matrix = Projection::create_perspective(fov, static_cast<float>(resolution.width) / resolution.height,
                                                       near_plane, far_plane, false);

    // setup compute shader
    std::vector<godot::String> args = {};//{"#define SCENE_ID " + scene_id};
    ray_marching_shader =
        new ComputeShader("res://addons/music_video_generator/src/shaders/ray_marcher.glsl", _rd, args);
    if (cone_marching_enabled)
        cone_marching_shader =
            new ComputeShader("res://addons/music_video_generator/src/shaders/cone_marcher.glsl", _rd, args);

    float r_cone_resolution_scale = 1.0f / cone_resolution_scale; // scale for cone resolution
    //--------- GENERAL BUFFERS ---------

    { // cone marching parameters buffer
        cone_marching_parameters.width = resolution.x * r_cone_resolution_scale;
        cone_marching_parameters.height = resolution.y * r_cone_resolution_scale;
        cone_marching_parameters.fov = fov;
        cone_marching_parameters.pixelConeAngle =
            std::tan(cone_marching_parameters.fov * 0.5f) / (0.5f * float(cone_marching_parameters.width));

        if (cone_marching_enabled)
            cone_marching_parameters_rid = cone_marching_shader->create_storage_buffer_uniform(
                cone_marching_parameters.to_packed_byte_array(), 0, 1);
    }

    { // ray marching parameters buffer
        render_parameters.width = resolution.x;
        render_parameters.height = resolution.y;
        render_parameters.backgroundColor =
            Vector4(background_color.r, background_color.g, background_color.b, background_color.a);
        render_parameters.fov = fov;
        render_parameters.coneResolutionScale = r_cone_resolution_scale;

        render_parameters_rid =
            ray_marching_shader->create_storage_buffer_uniform(render_parameters.to_packed_byte_array(), 0, 1);
    }

    { // camera buffer
        Vector3 camera_position = get_global_transform().get_origin();
        Projection VP = projection_matrix * get_global_transform().affine_inverse();
        Projection IVP = VP.inverse();

        Utils::projection_to_float(camera_parameters.vp, VP);
        Utils::projection_to_float(camera_parameters.ivp, IVP);
        camera_parameters.cameraPosition = Vector4(camera_position.x, camera_position.y, camera_position.z, 1.0f);
        camera_parameters.frame_index = 0;
        camera_parameters.nearPlane = near_plane;
        camera_parameters.farPlane = far_plane;
        camera_parameters.time = 0.0f;

        camera_parameters_rid =
            ray_marching_shader->create_storage_buffer_uniform(camera_parameters.to_packed_byte_array(), 1, 1);
        if (cone_marching_enabled)
            cone_marching_shader->add_existing_buffer(camera_parameters_rid,
                                                      RenderingDevice::UNIFORM_TYPE_STORAGE_BUFFER, 1, 1);
    }

    { // music data buffer
        music_data_rid = ray_marching_shader->create_storage_buffer_uniform(music_data.to_packed_byte_array(), 2, 1);
        if (cone_marching_enabled)
            cone_marching_shader->add_existing_buffer(music_data_rid, RenderingDevice::UNIFORM_TYPE_STORAGE_BUFFER, 2,
                                                      1);
    }

    Ref<RDTextureView> cone_texture_view = memnew(RDTextureView);
    { // cone marching texture
        auto cone_format =
            ray_marching_shader->create_texture_format(cone_marching_parameters.width, cone_marching_parameters.height,
                                                       RenderingDevice::DATA_FORMAT_R32G32_SFLOAT);
        cone_image = Image::create_empty(cone_marching_parameters.width, cone_marching_parameters.height, false,
                                         Image::FORMAT_RGF);
        cone_texture_rid = ray_marching_shader->create_image_uniform(cone_image, cone_format, cone_texture_view, 2, 0);

        if (cone_marching_enabled)
            cone_marching_shader->add_existing_buffer(cone_texture_rid, RenderingDevice::UNIFORM_TYPE_IMAGE, 0, 0);
    }

    Ref<RDTextureView> output_texture_view = memnew(RDTextureView);
    { // output texture
        auto output_format = ray_marching_shader->create_texture_format(
            render_parameters.width, render_parameters.height, RenderingDevice::DATA_FORMAT_R32G32B32A32_SFLOAT,
            RenderingDevice::TEXTURE_USAGE_STORAGE_BIT | RenderingDevice::TEXTURE_USAGE_CAN_UPDATE_BIT |
                RenderingDevice::TEXTURE_USAGE_CAN_COPY_FROM_BIT | RenderingDevice::TEXTURE_USAGE_SAMPLING_BIT);
        if (output_texture_rect == nullptr)
        {
            UtilityFunctions::printerr("No output texture set.");
            return;
        }
        output_image =
            Image::create_empty(render_parameters.width, render_parameters.height, false, Image::FORMAT_RGBAF);

        output_texture_rid =
            ray_marching_shader->create_image_uniform(output_image, output_format, output_texture_view, 0, 0);

        // output_texture = ImageTexture::create_from_image(output_image);
        output_texture.instantiate();
        output_texture->set_texture_rd_rid(output_texture_rid);
        output_texture_rect->set_texture(output_texture);
    }

    Ref<RDTextureView> depth_texture_view = memnew(RDTextureView);
    { // depth texture
        auto depth_format = ray_marching_shader->create_texture_format(
            render_parameters.width, render_parameters.height, RenderingDevice::DATA_FORMAT_R32_SFLOAT);
        depth_image = Image::create_empty(render_parameters.width, render_parameters.height, false, Image::FORMAT_RF);
        depth_texture_rid =
            ray_marching_shader->create_image_uniform(depth_image, depth_format, depth_texture_view, 1, 0);
    }

    ray_marching_shader->finish_create_uniforms();
    if (cone_marching_enabled)
        cone_marching_shader->finish_create_uniforms();
}

void RayMarchingCamera::clear_compute_shader()
{
}

void RayMarchingCamera::render()
{
    if (ray_marching_shader == nullptr || !ray_marching_shader->check_ready() ||
        cone_marching_enabled && (cone_marching_shader == nullptr || !cone_marching_shader->check_ready()))
        return;
    // update camera parameters
    {
        Vector3 camera_position = get_global_transform().get_origin();
        Projection VP = projection_matrix * get_global_transform().affine_inverse();
        Projection IVP = VP.inverse();

        Utils::projection_to_float(camera_parameters.vp, VP);
        Utils::projection_to_float(camera_parameters.ivp, IVP);
        camera_parameters.cameraPosition = Vector4(camera_position.x, camera_position.y, camera_position.z, 1.0f);
        camera_parameters.frame_index++;
        camera_parameters.time += get_process_delta_time();

        ray_marching_shader->update_storage_buffer_uniform(camera_parameters_rid,
                                                           camera_parameters.to_packed_byte_array());
    }

    // update music data
    {
        Vector4 current_magnitude_data = music_manager->get_current_magnitude_data();
        Vector4 cumulative_magnitude_data = music_manager->get_cumulative_magnitude_data();
        PackedFloat32Array spectrum_data = music_manager->get_spectrum_data();
        music_data.spectrum_count = std::min(static_cast<int>(spectrum_data.size()), MusicManager::MAX_SPECTRUM_SIZE);
        for (size_t i = 0; i < MusicManager::MAX_SPECTRUM_SIZE; i++)
        {
            music_data.spectrum[i] = i < spectrum_data.size() ? spectrum_data[i] : 0.0f;
        }
        
        music_data.current_magnitude_data = current_magnitude_data;
        music_data.cumulative_magnitude_data = cumulative_magnitude_data;
        ray_marching_shader->update_storage_buffer_uniform(music_data_rid, music_data.to_packed_byte_array());
    }

    // render
    if (cone_marching_enabled)
    {
        cone_marching_shader->compute({static_cast<int32_t>(std::ceil(cone_marching_parameters.width / 32.0f)),
                                       static_cast<int32_t>(std::ceil(cone_marching_parameters.height / 32.0f)), 1},
                                      false);
    }

    Vector2i Size = {render_parameters.width, render_parameters.height};

    ray_marching_shader->compute(
        {static_cast<int32_t>(std::ceil(Size.x / 32.0f)), static_cast<int32_t>(std::ceil(Size.y / 32.0f)), 1}, false);

    { // post processing
    }

    // output_image->set_data(Size.x, Size.y, false, Image::FORMAT_RGBAF,
    //                        ray_marching_shader->get_image_uniform_buffer(output_texture_rid));
    // output_texture->update(output_image);
}