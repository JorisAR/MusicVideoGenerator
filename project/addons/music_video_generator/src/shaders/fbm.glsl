#ifndef FBM_GLSL
#define FBM_GLSL

// based on https://iquilezles.org/articles/fbmsdf/

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

#endif // #ifndef FBM_GLSL