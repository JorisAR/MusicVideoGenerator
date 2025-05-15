#ifndef PRIMITIVES_GLSL
#define PRIMITIVES_GLSL

// Primitive functions
float sdBoxFrame( vec3 p, vec3 b, float e )
{
    p = abs(p) - b;
    vec3 q = abs(p + e) - e;
    return min(
        min(
            length(max(vec3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
            length(max(vec3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)
        ),
        length(max(vec3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0)
    );
}

float sdSphere( vec3 p, float r )
{
    return length(p) - r;
}

float sdPlane( vec3 p, vec3 n, float h )
{
  return dot(p,n) + h;
}

float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}


float sdBox2D(vec2 p, vec2 b) {
    vec2 d = abs(p) - b;
    return length(max(d, vec2(0.0))) + min(max(d.x, d.y), 0.0);
}

float sdTorus( vec3 p, vec2 t )
{
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

// Regular (hard) blending operations
float opUnion( float d1, float d2 ) {
    return min(d1, d2);
}

float opIntersection( float d1, float d2 ) {
    return max(d1, d2);
}

float opDifference( float d1, float d2 ) {
    return max(d1, -d2);
}

// Smooth blending functions
float smoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0 );
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

float smoothIntersection( float d1, float d2, float k ) {
    return -smoothUnion(-d1, -d2, k);
}

float smoothDifference( float d1, float d2, float k ) {
    return smoothIntersection(d1, -d2, k);
}

// Colors
struct SDFResult {
    vec3 color;
    float d;    
    float reflectivity;
    float alpha;
};

SDFResult sdfResult(float d, vec3 color) {
    SDFResult result;
    result.d = d;
    result.reflectivity = 0.0;
    result.alpha = 1.0;
    result.color = color;
    return result;
}

SDFResult mix(SDFResult a, SDFResult b, float t) {
    SDFResult result;
    result.d = mix(a.d, b.d, t);
    result.color = mix(a.color, b.color, t);
    result.reflectivity = mix(a.reflectivity, b.reflectivity, t);
    result.alpha = mix(a.alpha, b.alpha, t);
    return result;
}

SDFResult mixProperties(SDFResult a, SDFResult b, float t) {
    SDFResult result;
    result.color = mix(a.color, b.color, t);
    result.reflectivity = mix(a.reflectivity, b.reflectivity, t);
    result.alpha = mix(a.alpha, b.alpha, t);
    return result;
}


SDFResult smoothUnion(SDFResult a, SDFResult b, float k) {
    float h = clamp(0.5 + 0.5 * (b.d - a.d) / k, 0.0, 1.0);
    SDFResult result = mixProperties(b,a,h);
    result.d = mix(b.d, a.d, h) - k * h * (1.0 - h);
    return result;
}

SDFResult smoothIntersection(SDFResult a, SDFResult b, float k) {
    float h = clamp(0.5 + 0.5 * ((-b.d) - (-a.d)) / k, 0.0, 1.0);
    SDFResult result = mixProperties(b,a,h);
    result.d = - (mix(-b.d, -a.d, h) - k * h * (1.0 - h));
    return result;
}

SDFResult smoothDifference(SDFResult a, SDFResult b, float k) {
    float h = clamp(0.5 + 0.5 * ((b.d) - (-a.d)) / k, 0.0, 1.0);
    SDFResult result = mixProperties(b,a,h);
    result.d = - (mix(b.d, -a.d, h) - k * h * (1.0 - h));
    return result;
}

// ------------------------------------------------ Utils ------------------------------------------------

vec3 rotatePoint(vec3 p, vec3 r) {
    vec3 angles = radians(r);

    mat3 rotX = mat3(
        1, 0, 0,
        0, cos(angles.x), -sin(angles.x),
        0, sin(angles.x), cos(angles.x)
    );

    mat3 rotY = mat3(
        cos(angles.y), 0, sin(angles.y),
        0, 1, 0,
        -sin(angles.y), 0, cos(angles.y)
    );

    mat3 rotZ = mat3(
        cos(angles.z), -sin(angles.z), 0,
        sin(angles.z), cos(angles.z), 0,
        0, 0, 1
    );

    mat3 rotationMatrix = rotZ * rotY * rotX;
    return rotationMatrix * p;
}


vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 gridColoring(vec3 pos, vec3 a, vec3 b)
{
    bool c = (int(floor(pos.x)) % 2 == 0) ^^ (int(floor(pos.y)) % 2 == 0) ^^ (int(floor(pos.z)) % 2 == 0);
    return c ? a : b;
}

#endif // PRIMITIVES_GLSL
