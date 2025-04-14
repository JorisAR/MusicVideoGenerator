#[compute]
#version 460

#include "Test.glsl" 

struct Sphere {
    vec3 position;
    float radius;
};


struct Light {
    vec4 position;
    vec4 color;
};

layout(std430, set = 0, binding = 0) restrict buffer Params {
    int width;
    int height;
    float fov;
    float fov2;
    vec4 cameraPosition;
    vec4 cameraUp;
    vec4 cameraRight;
    vec4 cameraFront;

} params;

layout(std430, set = 1, binding = 0) restrict buffer SphereBuffer {
    vec4 numSpheres;
    Sphere spheres[100];
} sphereBuffer;


layout(set = 0, binding = 1, r8) restrict uniform writeonly image2D outputImage;

float distanceToSphere(vec3 point, Sphere sphere) {
    return length(point - sphere.position.xyz) - sphere.radius;
}

// float distanceToBox(vec3 point, Box box) {
//     vec3 d = max(box.min - point, 0.0) + max(point - box.max, 0.0);
//     return length(d);
// }

float sdPlane(vec3 p, vec3 n, float h)
{
    // n must be normalized
    return dot(p, n) + h;
}

float sdBox(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// https://iquilezles.org/articles/smin
float smin(float a, float b, float k)
{
    float h = max(k-abs(a-b), 0.0);
    return min(a, b) - h*h*0.25/k;
}

// https://iquilezles.org/articles/smin
float smax(float a, float b, float k)
{
    float h = max(k-abs(a-b), 0.0);
    return max(a, b) + h*h*0.25/k;
}


vec3 menger_cube(in vec3 p)
{
    float d = sdBox(p, vec3(1.0));
    vec3 res = vec3(d, 1.0, 0.0);

    float s = 1.0;
    for (int m=0; m<3; m++)
    {
        vec3 a = mod(p*s, 2.0)-1.0;
        s *= 3.0;
        vec3 r = abs(1.0 - 3.0*abs(a));

        float da = max(r.x, r.y);
        float db = max(r.y, r.z);
        float dc = max(r.z, r.x);
        float c = (min(da, min(db, dc))-1.0)/s;

        if (c>d)
        {
            d = c;
            res = vec3(d, 0.2*da*db*dc, (1.0+float(m))/4.0);
        }
    }

    return res;
}


float sceneDistance(vec3 point) {
    float minDist = 1e10;
        for (int i = 0; i < sphereBuffer.numSpheres.x; i++) {
            float dist = distanceToSphere(point, sphereBuffer.spheres[i]);
            minDist = smin(minDist, dist, 0.5);
        }
        return minDist;
    //return map(point).x;
}

vec3 calculateNormal( in vec3 point ) // for function f(p)
{
    const float h = 0.0001;      // replace by an appropriate value

    vec3 n = vec3(0.0);
    for( int i=0; i<4; i++ )
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*sceneDistance(point+e*h).x;
    }
    return normalize(n);
}

float softshadow(in vec3 ro, in vec3 rd, float k)
{
    float res = 1.0;
    float t = 0.01;
    for (int i=0; i<50 && t<100; i++)
    {
        float h = sceneDistance(ro + rd*t);
        if (h<0.001)
        return 0.0;
        res = min(res, k*h/t);
        t += h;
    }
    return res;
}

float raymarch(vec3 origin, vec3 direction) {
    float t = 0.0;
    for (int i = 0; i < 100 && t<100; i++) {
        vec3 p = origin + t * direction;
        float dist = sceneDistance(p);
        if (dist < 0.01) {
            return t;
        }
        t += dist;
    }
    return -1.0;
}

vec3 phongShading(vec3 cameraPosition, vec3 position, vec3 normal, Light light) { 
    vec3 lightDir = normalize(light.position.xyz - position); 
    float diff = max(dot(normal, lightDir), 0.0); 
    vec3 diffuse = diff * light.color.rgb; 
    vec3 viewDir = normalize(-cameraPosition); 
    vec3 reflectDir = reflect(lightDir, normal); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); 
    vec3 specular = spec * light.color.rgb; 
    return diffuse + specular; 
}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= params.width || pos.y >= params.height) return;

    vec3 cameraPosition = params.cameraPosition.xyz;
    float fov = 45.0;

    vec2 uv = (vec2(pos) / vec2(params.width, params.height)) * 2.0 - 1.0;
    float aspectRatio = float(params.width) / float(params.height);
    vec3 rayDir = normalize(uv.x * params.cameraRight.xyz * aspectRatio + -uv.y * params.cameraUp.xyz - params.cameraFront.xyz * 2.5);

    float t = raymarch(cameraPosition, rayDir);
    vec3 color = vec3(0.0f);

    if (t > 0.0) {
        vec3 position = cameraPosition + t * rayDir;
        vec3 normal = calculateNormal(position);
        Light light = Light(vec4(50, 50, 50, 0), vec4(1.0));
        float shadow = softshadow(cameraPosition, normalize(light.position.xyz - position), 32);
        color = phongShading(cameraPosition, position, normal, light) * shadow;
        //color = vec3(1.0f);
    }

    imageStore(outputImage, pos, vec4(color, 1.0));
}
