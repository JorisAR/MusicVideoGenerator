#ifndef SCENE_GLSL
#define SCENE_GLSL

float sdScene( vec3 p )
{
    return length(p) - 1.0f;
    // return sdSphere( p, 1.0 );
}

#endif // SCENE_GLSL