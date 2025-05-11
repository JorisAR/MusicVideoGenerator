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
float opSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0 );
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

float opSmoothIntersection( float d1, float d2, float k ) {
    return -opSmoothUnion(-d1, -d2, k);
}

float opSmoothDifference( float d1, float d2, float k ) {
    return opSmoothIntersection(d1, -d2, k);
}

// Colors
struct SDFResult {
    float d;
    vec3 color;
};

SDFResult opSmoothUnionSDF(SDFResult a, SDFResult b, float k) {
    float h = clamp(0.5 + 0.5 * (b.d - a.d) / k, 0.0, 1.0);
    SDFResult result;
    result.d = mix(b.d, a.d, h) - k * h * (1.0 - h);
    result.color = mix(b.color, a.color, h);
    return result;
}

SDFResult opSmoothIntersectionSDF(SDFResult a, SDFResult b, float k) {
    float h = clamp(0.5 + 0.5 * ((-b.d) - (-a.d)) / k, 0.0, 1.0);
    SDFResult result;
    result.d = - (mix(-b.d, -a.d, h) - k * h * (1.0 - h));
    result.color = mix(b.color, a.color, h);
    return result;
}

SDFResult opSmoothDifferenceSDF(SDFResult a, SDFResult b, float k) {
    SDFResult negB;
    negB.d = -b.d;
    negB.color = b.color;
    return opSmoothIntersectionSDF(a, negB, k);
}

#endif // PRIMITIVES_GLSL
