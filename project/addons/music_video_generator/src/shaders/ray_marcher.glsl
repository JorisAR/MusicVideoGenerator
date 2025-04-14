#[compute]
#version 460

#include "primitives.glsl"
#include "scenes/scene.glsl"

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

layout(set = 0, binding = 0, rgba8) restrict uniform writeonly image2D outputImage;
layout(set = 0, binding = 1, r32f) restrict uniform writeonly image2D depthBuffer;

layout(std430, set = 0, binding = 2) restrict buffer Params {
    vec4 background; //rgb, brightness
    int width;
    int height;
    float fov;
    uint triangleCount;
    uint blas_count;
} params;

layout(std430, set = 0, binding = 3) restrict buffer Camera {
    mat4 view_projection;
    mat4 inv_view_projection;
    vec4 position;
    uint frame_index;
    float near;
    float far;
    float padding;
} camera;

// ----------------------------------- FUNCTIONS -----------------------------------


vec2 pcg2d(inout uvec2 seed) {
	// PCG2D, as described here: https://jcgt.org/published/0009/03/02/
	seed = 1664525u * seed + 1013904223u;
	seed.x += 1664525u * seed.y;
	seed.y += 1664525u * seed.x;
	seed ^= (seed >> 16u);
	seed.x += 1664525u * seed.y;
	seed.y += 1664525u * seed.x;
	seed ^= (seed >> 16u);
	// Multiply by 2^-32 to get floats
	return vec2(seed) * 2.32830643654e-10; 
}

uvec2 prng_seed(vec2 pos, uint frame) {
    uvec2 seed = uvec2(pos.xy);
    seed = seed * 0x9e3779b9u + uvec2(frame);
    seed ^= seed >> 16u;
    return seed * 0x9e3779b9u;
}

vec2 box_muller(vec2 rands) {
    float R = sqrt(-2.0f * log(rands.x));
    float theta = 6.2831853f * rands.y;
    return vec2(cos(theta), sin(theta));
}

vec3 sampleSky(const vec3 direction) {
    float t = 0.5 * (direction.y + 1.0);
    return mix(vec3(0.95), vec3(0.9, 0.94, 1.0), t) * 1.0f;
}


float raymarch(vec3 origin, vec3 direction) {
    float t = 0.0;
    for (int i = 0; i < 100 && t<100; i++) {
        vec3 p = origin + t * direction;
        float dist = sdScene(p);
        if (dist < 0.01) {
            return t;
        }
        t += dist;
    }
    return -1.0;
}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= params.width || pos.y >= params.height) return;

    uvec2 seed = prng_seed(gl_GlobalInvocationID.xy, camera.frame_index);
    vec2 screen_uv = vec2(pos + 0.5) / vec2(params.width, params.height); // + box_muller(pcg2d(seed) * 0.25)

    vec4 ndc = vec4(screen_uv * 2.0 - 1.0, 0.0, 1.0);
    ndc.y = -ndc.y;

    vec4 world_pos = inverse(camera.view_projection) * ndc;
    world_pos /= world_pos.w;
    vec3 ray_origin = camera.position.xyz;
    vec3 ray_dir = normalize(world_pos.xyz - ray_origin);

    float t = raymarch(ray_origin, ray_dir);
    vec3 color = vec3(0.0f);
    color = ray_dir * 0.5 + 0.5;

    if (t >= 0.0) {
        vec3 position = ray_origin + t * ray_dir;
        // vec3 normal = calculateNormal(position);
        // Light light = Light(vec4(50, 50, 50, 0), vec4(1.0));
        // float shadow = softshadow(cameraPosition, normalize(light.position.xyz - position), 32);
        // color = phongShading(cameraPosition, position, normal, light) * shadow;
        color = vec3(1.0f);
    }

    // depth = camera.far / (camera.far - camera.near) * (1.0 - camera.near / depth);
    float depth = 0.0f;
    imageStore(outputImage, pos, vec4(color, 1.0));
    imageStore(depthBuffer, pos, vec4(depth, 0.0, 0.0, 0.0));
}