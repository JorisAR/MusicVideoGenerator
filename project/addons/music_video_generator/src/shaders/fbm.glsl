#ifndef FBM_GLSL
#define FBM_GLSL

// based on https://iquilezles.org/articles/fbmsdf/

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float hash(vec3 p) {
    p = fract(p * vec3(443.897, 397.297, 491.423));
    p += dot(p, p.yxz + 19.19);
    return fract((p.x + p.y) * p.z);
}


float sdFbmSph( ivec3 i, vec3 f, ivec3 c )
{
   // random radius at grid vertex i+c
   float rad = 0.5*hash(vec3(i+c));
   // distance to sphere at grid vertex i+c
   return length(f-vec3(c)) - rad; 
}

float sdFbmBase( vec3 p )
{
   ivec3 i = ivec3(floor(p));
    vec3 f =       fract(p);
   // distance to the 8 corners spheres
   return min(min(min(sdFbmSph(i,f,ivec3(0,0,0)),
                      sdFbmSph(i,f,ivec3(0,0,1))),
                  min(sdFbmSph(i,f,ivec3(0,1,0)),
                      sdFbmSph(i,f,ivec3(0,1,1)))),
              min(min(sdFbmSph(i,f,ivec3(1,0,0)),
                      sdFbmSph(i,f,ivec3(1,0,1))),
                  min(sdFbmSph(i,f,ivec3(1,1,0)),
                      sdFbmSph(i,f,ivec3(1,1,1)))));
}


float sdFbm( vec3 p, float d, int octaves )
{
   float s = 5.0;

   for( int i=0; i<octaves; i++ )
   {
       // evaluate new octave
       float n = s*sdFbmBase(p);
       n = smoothDifference(n,0.1*s - d,0.3*s);
       d = smoothUnion(n,d      ,0.3*s);
	
       // prepare next octave
       p = mat3( 0.00, 1.60, 1.20,
                -1.60, 0.72,-0.96,
                -1.20,-0.96, 1.28 )*p;
       s = 0.5*s;
   }
   return d;
}

// alternative simple heightmap style terrain
vec3 terrainHeightAndGradient(vec2 p) {
    float h = 0.0;
    vec2 grad = vec2(0.0);
    float frequency = 0.08;
    float amplitude = 2.0;
    const int NUM_WAVES = 20;
    
    for (int i = 0; i < NUM_WAVES; i++) {
        float angle = float(i) * (3.141592653 / float(NUM_WAVES)) + camera.time * 0.01 * float(i) / float(NUM_WAVES);
        vec2 dir = vec2(cos(angle), sin(angle));

        float arg = dot(p, dir) * frequency + float(i);
        float s = sin(arg);
        float c = cos(arg);
        
        h += amplitude * s;
        grad += amplitude * frequency * c * dir;
        
        frequency *= 1.02;
        amplitude *= 0.96;
    }
    
    return vec3(h, grad.x, grad.y);
}

float sdTerrainCurved(vec3 p) {
    vec3 hg = terrainHeightAndGradient(p.xz);
    float h = 2.0 * hg.x * (0.0 + 2.0 * clamp(abs(p.x) * 0.005, 0, 1));
    vec2 grad = vec2(hg.y, hg.z);
    
    float normFactor = sqrt(1.0 + dot(grad, grad));
    
    return (p.y - h) / normFactor;
}



#endif // #ifndef FBM_GLSL