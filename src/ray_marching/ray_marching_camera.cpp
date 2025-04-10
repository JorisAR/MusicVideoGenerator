#include "ray_marching_camera.h"

void RayMarchingCamera::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_fov"), &RayMarchingCamera::get_fov);
    ClassDB::bind_method(D_METHOD("set_fov", "value"), &RayMarchingCamera::set_fov);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fov"), "set_fov", "get_fov");

    // ClassDB::bind_method(D_METHOD("get_num_bounces"), &RayMarchingCamera::get_num_bounces);
    // ClassDB::bind_method(D_METHOD("set_num_bounces", "value"), &RayMarchingCamera::set_num_bounces);
    // ADD_PROPERTY(PropertyInfo(Variant::INT, "num_bounces"), "set_num_bounces", "get_num_bounces");

    ClassDB::bind_method(D_METHOD("get_output_texture"), &RayMarchingCamera::get_output_texture);
    ClassDB::bind_method(D_METHOD("set_output_texture", "value"), &RayMarchingCamera::set_output_texture);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "output_texture", PROPERTY_HINT_NODE_TYPE, "TextureRect"),
                 "set_output_texture", "get_output_texture");
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

void RayMarchingCamera::set_output_texture(TextureRect *value)
{
    output_texture_rect = value;
}


void RayMarchingCamera::init()
{
    //we want to use one RD for all shaders relevant to the camera.
    _rd = RenderingServer::get_singleton()->create_local_rendering_device();


    //get resolution
    auto resolution = DisplayServer::get_singleton()->window_get_size();

    { // setup parameters
        render_parameters.width = resolution.x;
        render_parameters.height = resolution.y;
        render_parameters.fov = fov;
        projection_matrix = Projection::create_perspective(fov, static_cast<float>(render_parameters.width) / render_parameters.height, 0.01f, 1000.0f, false);
        //camera.set_camera_transform(get_global_transform().affine_inverse(), projection_matrix);
    }

    // setup compute shader
    cs = new ComputeShader("res://addons/music_video_generator/src/shaders/ray_marcher.glsl", _rd, {"#define TESTe"});
    //--------- GENERAL BUFFERS ---------
    { // input general buffer
        render_parameters_rid = cs->create_storage_buffer_uniform(render_parameters.to_packed_byte_array(), 2, 0);
        //camera_rid = cs->create_storage_buffer_uniform(camera.to_packed_byte_array(), 3, 0);
    }

    Ref<RDTextureView> output_texture_view = memnew(RDTextureView);
    { // output texture
        auto output_format = cs->create_texture_format(render_parameters.width, render_parameters.height, RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
        if (output_texture_rect == nullptr)
        {
            UtilityFunctions::printerr("No output texture set.");
            return;
        }
        output_image = Image::create(render_parameters.width, render_parameters.height, false, Image::FORMAT_RGBA8);
        output_texture = ImageTexture::create_from_image(output_image);
        output_texture_rect->set_texture(output_texture);
        
        output_texture_rid = cs->create_image_uniform(output_image, output_format, output_texture_view, 0, 0);
    }

    Ref<RDTextureView> depth_texture_view = memnew(RDTextureView);
    { // depth texture
        auto depth_format = cs->create_texture_format(render_parameters.width, render_parameters.height, RenderingDevice::DATA_FORMAT_R32_SFLOAT);
        depth_image = Image::create(render_parameters.width, render_parameters.height, false, Image::FORMAT_RF);        
        depth_texture_rid = cs->create_image_uniform(depth_image, depth_format, depth_texture_view, 1, 0);
    }

    cs->finish_create_uniforms();
}

void RayMarchingCamera::clear_compute_shader()
{
}

void RayMarchingCamera::render()
{
    if (cs == nullptr || !cs->check_ready())
        return;
    // update rendering parameters
    //camera.set_camera_transform(get_global_transform(), projection_matrix);
    // camera.frame_index++;
    // cs->update_storage_buffer_uniform(camera_rid, camera.to_packed_byte_array());

    // render
    Vector2i Size = {render_parameters.width, render_parameters.height};
    cs->compute({static_cast<int32_t>(std::ceil(Size.x / 32.0f)), static_cast<int32_t>(std::ceil(Size.y / 32.0f)), 1});
    
    { // post processing

    }
    
    output_image->set_data(Size.x, Size.y, false, Image::FORMAT_RGBA8,
                           cs->get_image_uniform_buffer(output_texture_rid));
    output_texture->update(output_image);
    // load texture data?
}