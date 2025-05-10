#ifndef SCENE_GLSL
#define SCENE_GLSL

vec2 offset(vec2 q, float p) {
    q.x += -2.5 + (1.0 + sin((p) * 0.1 + camera.time * 0.5)) * 1.0;
    return q;
}

float sdLinesScene(in vec3 p)
{
    const float period = 5.0; 
    float halfThickness = 0.5 + 0.25 * music_data.raw.x;    

    const vec2[4] offsets = {vec2(0.0, 0.0), vec2(period, 0.0), vec2(0.0, period), vec2(period, period)};
    vec3 q = mod(p, period) - 0.5 * period;
    float d = 1e10;
    for (int i = 0; i < 4; i++) {
        d = min(d, sdBox2D(offset(vec2(q.y, q.z) + offsets[i], p.z), vec2(halfThickness)));    
        d = min(d, sdBox2D(offset(vec2(q.x, q.z) + offsets[i], p.z), vec2(halfThickness)));
        d = min(d, sdBox2D(offset(vec2(q.x, q.y) + offsets[i], p.z), vec2(halfThickness)));
    }

    return d;
}

float sdMengerScene(in vec3 p)
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