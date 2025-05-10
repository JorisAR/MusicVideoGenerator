#ifndef PRIMITIVES_GLSL
#define PRIMITIVES_GLSL

// Existing primitives

// Box frame SDF
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

// Sphere SDF
float sdSphere( vec3 p, float r )
{
    return length(p) - r;
}

// Box SDF
float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// 2D Box SDF
float sdBox2D(vec2 p, vec2 b) {
    vec2 d = abs(p) - b;
    return length(max(d, vec2(0.0))) + min(max(d.x, d.y), 0.0);
}

// Additional primitive: Torus
// 't.x' is the major (tubular) radius and 't.y' is the minor (tube) radius.
float sdTorus( vec3 p, vec2 t )
{
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}


// Regular (hard) blending operations

// Regular Union: the minimum distance.
float opUnion( float d1, float d2 ) {
    return min(d1, d2);
}

// Regular Intersection: the maximum distance.
float opIntersection( float d1, float d2 ) {
    return max(d1, d2);
}

// Regular Difference: subtracts the second shape from the first.
float opDifference( float d1, float d2 ) {
    return max(d1, -d2);
}


// Smooth blending functions

// Smooth Union: blends two distances over a blending factor 'k'.
// 'k' determines how smooth the transition is.
float opSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0 );
    // mix(x, y, a) returns a linear interpolation between x and y using factor a.
    return mix(d2, d1, h) - k * h * (1.0 - h);
}

// Smooth Intersection: use de Morgan's law by inverting inputs/outputs.
float opSmoothIntersection( float d1, float d2, float k ) {
    return -opSmoothUnion(-d1, -d2, k);
}

// Smooth Difference: subtract d2 from d1 with a smooth transition.
float opSmoothDifference( float d1, float d2, float k ) {
    return opSmoothIntersection(d1, -d2, k);
}

#endif // PRIMITIVES_GLSL
