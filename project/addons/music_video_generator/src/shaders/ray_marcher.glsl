#[compute]
#version 460

#define MAX_RAY_MARCHING_STEPS 512

// #define DISABLE_SHADOWS
// #define DISABLE_PHONG_SHADING
// #define DISABLE_REFLECTIONS

#ifndef SCENE_ID
#define SCENE_ID 5
#endif


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
layout(set = 0, binding = 2, r32f) restrict uniform image2D coneBuffer;


layout(std430, set = 1, binding = 0) restrict buffer Params {
    vec4 background; //rgb, brightness
    int width;
    int height;
    float fov;
    float cone_resolution_scale;
} params;

layout(std430, set = 1, binding = 1) restrict buffer Camera {
    mat4 view_projection;
    mat4 inv_view_projection;
    vec4 position;
    uint frame_index;
    float near;
    float far;
    float time;
} camera;

layout(std430, set = 1, binding = 2) restrict buffer MusicData {
    vec4 current;
    vec4 cumulative;
    float spectrum[64];
    int spectrum_count;
} music_data;

// ----------------------------------- SHAPES -----------------------------------
#include "primitives.glsl"
#include "fbm.glsl"
#include "scenes.glsl"

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

vec3 sampleSky(vec3 direction) {
    float t = 0.5 * (direction.y + 1.0);
#if SCENE_ID == 1
    return mix(0.05, 0.25, t) * vec3(0.2, 0.02, 0.3);
#elif SCENE_ID == 2
    // return mix(vec3(0.95), vec3(0.9, 0.94, 1.0), t) * 1.0f;
#elif SCENE_ID == 3
    return mix(vec3(0.2, 0.3, 0.98), vec3(0.4, 0.5, 1.0), t) * 1.0f;
#endif
    return vec3(0.0);
}


vec3 geometry_color(const vec3 position) {
    const vec3 red = vec3(0.2, 0.2, 0.4);
    const vec3 green = vec3(0.4, 1.0, 0.2);
    const vec3 blue = vec3(0.2, 0.4, 0.2);

    float t = length(position) * 0.1 + camera.time * 0.2;
    t = mod(t, 3.0);
    if (t < 1.0) {
        return mix(red, green, t);
    } else if (t < 2.0) {
        return mix(green, blue, t - 1.0);
    } else {
        return mix(blue, red, t - 2.0);
    }
}

SDFResult ray_march(vec3 origin, vec3 direction, out vec3 hit_position, out float min_t, out int steps) {
    float t = 0.0;
    min_t = camera.far;
    steps = 0;
    
    while (steps < MAX_RAY_MARCHING_STEPS && t < camera.far) {
        steps++;
        hit_position = origin + t * direction;
        SDFResult sceneResult = sdScene(hit_position);
        float d = sceneResult.d;
        min_t = min(min_t, d);

        if (d < 0.0001) {
            // sceneResult.d = t;
            return sceneResult;
        }
        t += d;
    }

    // vec3 sky_color = steps >= MAX_RAY_MARCHING_STEPS ? vec3(0) : sampleSky(direction);
    vec3 sky_color = sampleSky(direction);
    return sdfResult(-1.0, sky_color);
}


// ----------------------------------- LIGHTING FUNCTIONS -----------------------------------

// Phong shading using a directional light.
// viewPos: camera position
// hitPos: position on the surface from the raymarch
// normal: surface normal at hitPos
// lightDir: normalized directional light vector
vec3 phongShading(vec3 viewPos, vec3 hitPos, vec3 normal, vec3 lightDir) {
    vec3 lightColor = vec3(1.0); // white light
    // Ambient component
    vec3 ambient = 0.1 * lightColor;
    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular component
    vec3 viewDir = normalize(viewPos - hitPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); // adjust 16.0 for shininess
    vec3 specular = spec * lightColor;
    return ambient + diffuse + specular;
}

vec3 calculateNormal( in vec3 point )
{
    const float h = 0.1;

    vec3 n = vec3(0.0);
    for( int i=0; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*sdScene(point+e*h).d;
    }
    return normalize(n);
}

float softshadow(in vec3 ro, in vec3 rd, float k)
{
    float res = 1.0;
    float t = 0.01;
    for (int i=0; i<50 && t<100; i++)
    {
        float h = sdScene(ro + rd*t).d;
        if (h<0.001)
        return 0.0;
        res = min(res, k*h/t);
        t += h;
    }
    return res;
}



// ----------------------------------- MAIN -----------------------------------
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= params.width || pos.y >= params.height) return;

    uvec2 seed = prng_seed(gl_GlobalInvocationID.xy, camera.frame_index);
    vec2 screen_uv = vec2(pos + 0.5) / vec2(params.width, params.height);
    
    vec4 ndc = vec4(screen_uv * 2.0 - 1.0, 0.0, 1.0);
    ndc.y = -ndc.y;

    // Using the inverse view-projection matrix from the camera buffer.
    vec4 world_pos = camera.inv_view_projection * ndc;
    world_pos /= world_pos.w;
    vec3 ray_origin = camera.position.xyz;
    vec3 ray_dir = normalize(world_pos.xyz - ray_origin);

    vec4 cone = imageLoad(coneBuffer, ivec2(clamp(pos * params.cone_resolution_scale, vec2(0), vec2(params.cone_resolution_scale * params.width - 1, params.cone_resolution_scale * params.height - 1))));
    float init_t = cone.x;
    ray_origin += ray_dir * max(init_t, camera.near);

    // imageStore(outputImage, pos, vec4(vec3(init_t), 1.0));
    // return;


    vec3 directional_light = normalize(vec3(-1.0, 1.0, 0.0));
    
    int steps = 0;
    float min_t = camera.far;

    vec3 hitPos = vec3(0.0);
    SDFResult result = ray_march(ray_origin, ray_dir, hitPos, min_t, steps);
    steps += int(cone.y); //add steps taken during cone marching

    float t = result.d;
    
    vec3 color = result.color;
    vec3 sky_color = sampleSky(ray_dir);

    if (t >= 0.0) {
        float linear_depth = (t - camera.near) / (camera.far - camera.near);
        vec3 normal = calculateNormal(hitPos);

#ifndef DISABLE_REFLECTIONS
        //reflections
        if(result.reflectivity > 0.0) {
            vec3 reflectDir = reflect(ray_dir, normal);
            int reflectSteps = 0;
            vec3 reflectHitPos = vec3(0);
            float reflectMinT = camera.far;
            vec3 reflectedColor = ray_march(hitPos + normal * 0.001, reflectDir, reflectHitPos, reflectMinT, reflectSteps).color;
            color =  mix(color, reflectedColor, result.reflectivity);
        }
#endif


#ifndef DISABLE_SHADOWS
        color = color * softshadow(hitPos + normal * 0.001, directional_light, 50.0);
#endif

#ifndef DISABLE_PHONG_SHADING
        color = color * phongShading(camera.position.xyz, hitPos, normal, directional_light);
#endif
        // color = geometry_color(hitPos);
        color = mix(color, sky_color, (steps / float(MAX_RAY_MARCHING_STEPS)));
        // color = mix(color, sky_color, pow(linear_depth, 1.0));
    }

    // Writing the final color and dummy depth.
    imageStore(outputImage, pos, vec4(color, 1.0));
    imageStore(depthBuffer, pos, vec4(0.0, 0.0, 0.0, 0.0));
}
