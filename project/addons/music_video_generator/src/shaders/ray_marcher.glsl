#[compute]
#version 460

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

// layout(std430, set = 0, binding = 3) restrict buffer Camera {
//     mat4 vp;
//     mat4 ivp;
//     vec4 position;
//     uint frame_index;
//     float near;
//     float far;
// } camera;

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

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= params.width || pos.y >= params.height) return;
    // uvec2 seed = prng_seed(gl_GlobalInvocationID.xy, camera.frame_index);

    vec2 screenPos = vec2(pos) / vec2(params.width, params.height) * 2.0 - 1.0;
    // vec2 screenPos = vec2(pos + box_muller(pcg2d(seed) * 0.25)) / vec2(params.width, params.height) * 2.0 - 1.0;
    vec4 ndc = vec4(screenPos.x, -screenPos.y, 1.0, 1.0);
    // vec4 worldPos = camera.ivp * ndcPos;
    // worldPos /= worldPos.w;

    // Ray ray;
    // ray.o = camera.position.xyz;

    // ray.d = normalize(worldPos.xyz - camera.position.xyz);
    // ray.rD = 1.0 / ray.d;
    // float depth = camera.far;

    vec3 radiance = vec3(ndc.x, ndc.y, 0.0) * 0.5 + 0.5;

    // depth = camera.far / (camera.far - camera.near) * (1.0 - camera.near / depth);
    float depth = 0.0f;
    imageStore(outputImage, pos, vec4(radiance, 1.0));
    imageStore(depthBuffer, pos, vec4(depth, 0.0, 0.0, 0.0));
}