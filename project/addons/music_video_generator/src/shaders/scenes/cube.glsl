#ifndef SCENE_GLSL
#define SCENE_GLSL


float sdScene(in vec3 p)
{
    p *= 0.1;
    float d = sdBox(p, vec3(1.0));
    vec3 res = vec3(d, 1.0, 0.0);

    float s = 1.0;
    for (int m=0; m<4; m++)
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

    return res.x;
}

#endif // SCENE_GLSL