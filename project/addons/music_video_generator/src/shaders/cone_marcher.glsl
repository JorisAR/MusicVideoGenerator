#[compute]
#version 460

#include "primitives.glsl"
#include "scenes/cube.glsl"

// ----------------------------------- STRUCTS -----------------------------------

struct Light {
    vec4 position;
    vec4 color;
};

struct Ray {
    vec3 d;
    vec3 o;
    vec3 rD;
};

// ----------------------------------- GENERAL STORAGE -----------------------------------

layout(set = 0, binding = 0, r32f) restrict uniform image2D coneBuffer;

layout(std430, set = 1, binding = 0) restrict buffer Params {
    int width;
    int height;
    float fov;
    float pixelConeAngle;
} params;

layout(std430, set = 1, binding = 1) restrict buffer Camera {
    mat4 view_projection;
    mat4 inv_view_projection;
    vec4 position;
    uint frame_index;
    float near;
    float far;
    float padding;
} camera;

layout(std430, set = 1, binding = 2) restrict buffer MusicData {
    vec4 raw;
} music_data;


// ----------------------------------- CONE MARCH FUNCTION -----------------------------------

float coneMarch(vec3 origin, vec3 direction) {
    float t = 0.0;
    // float pixelConeAngle = tan(params.fov * 0.5) / (0.5 * float(params.width));
    float pixelConeAngle = params.pixelConeAngle;
    
    for (int i = 0; i < 100 && t < camera.far; i++) {
        vec3 p = origin + t * direction;
        float dist = sdScene(p);
        float coneRadius = max(0.005, t * pixelConeAngle);
        
        if (dist < coneRadius) {
            return t;
        }
        t += dist;
    }
    return -1.0;
}


// ----------------------------------- MAIN -----------------------------------

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= params.width || pos.y >= params.height) return;
    vec2 screen_uv = (vec2(pos) + 0.5) / vec2(params.width, params.height);
    vec4 ndc = vec4(screen_uv * 2.0 - 1.0, 0.0, 1.0);
    ndc.y = -ndc.y;

    // Using the camera's inverse view-projection to determine world space position.
    vec4 world_pos = camera.inv_view_projection * ndc;
    world_pos /= world_pos.w;
    vec3 ray_origin = camera.position.xyz;
    vec3 ray_dir = normalize(world_pos.xyz - ray_origin);

    float t = coneMarch(ray_origin, ray_dir);
    if (t >= 0.0) {
        vec3 hitPos = ray_origin + t * ray_dir;
        imageStore(coneBuffer, pos, vec4(t));
    } else {
        imageStore(coneBuffer, pos, vec4(0.0));
    }   
}
