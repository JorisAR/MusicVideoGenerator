#ifndef SCENE_GLSL
#define SCENE_GLSL

vec2 offset(vec2 q, float p) {
    q.x += -2.5 + (1.0 + sin((p) * 0.1 + camera.time * 0.5)) * 1.0;
    return q;
}

SDFResult sdGrid(in vec3 p)
{
    const float period = 5.0; 
    float halfThickness = 0.5 + 0.25 * music_data.current.x;    

    const vec2[4] offsets = {vec2(0.0, 0.0), vec2(period, 0.0), vec2(0.0, period), vec2(period, period)};
    vec3 q = mod(p, period) - 0.5 * period;
    float d = 1e10;
    for (int i = 0; i < 4; i++) {
        d = min(d, sdBox2D(offset(vec2(q.y, q.z) + offsets[i], p.z), vec2(halfThickness)));    
        d = min(d, sdBox2D(offset(vec2(q.x, q.z) + offsets[i], p.z), vec2(halfThickness)));
        d = min(d, sdBox2D(offset(vec2(q.x, q.y) + offsets[i], p.z), vec2(halfThickness)));
    }

    return sdfResult(d, vec3(0.5, 0.5, 0.5));
}

SDFResult sdComplexGrid(in vec3 p)
{    
    SDFResult box = sdfResult(sdBox(p, vec3(40.0)), vec3(0.5, 0.5, 1.0));
    SDFResult sphere = sdfResult(sdSphere(p, 20.0), vec3(1.0, 0.5, 0.5));
    SDFResult grid = sdGrid(p);

    SDFResult res = smoothDifference(sdGrid(p), box, 2.0);
    res = smoothUnion(res, sphere, 0.5);
    return res;
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

// ----------------------- synthwave demo scene -----------------------

SDFResult sdSunScene(in vec3 p)
{
    float r = 100.0;
    float h = (p.y / r) * 0.5 + 0.5;
    vec3 color = mix(vec3(3.0, 0.2, 1.0), vec3(4.0, 1.0, 0.0), h);
    float d = sdSphere(p, r);

    float d_plane = (sin(0.2 * (p.y * 1.0 + -camera.time * 2.0)) * 0.5 + 0.5 - pow(h + 0.1, 0.1));
    // d_plane = mix(d, d_plane, 1 - h);
    d = max(d_plane, d);

    return sdfResult(d, color * 1.5);
}

//this uses some poor but acceptable "noise" functions
SDFResult sdBubblesScene(in vec3 p)
{
    p.z -= 5.0 * camera.time;

    float grid_frequency = 100.0f;    
    vec2 p_id = floor(p.xz / grid_frequency);  

    p.xz = mod(p.xz, vec2(grid_frequency));    
    p.xz -= vec2(0.5 * (grid_frequency));
    p.y -= 5;    

    float r = 5.0;
    
    vec2 rng = //some noise function
        vec2(sin(0.2 * (p_id.x * 768.5847)) * 0.5 + 0.5,
        sin(0.2 * (p_id.y * 357.4378)) * 0.5 + 0.5);
    float rng_float = rng.x + rng.y;
    p.y -= rng_float * 10.0;

    SDFResult result = sdfResult(1e30, vec3(0));
    float n = mod(rng_float * 5473.0, 4.0) + 2.0;
    for (int i = 0; i < n; i++) {

        float i_rng = (fract(float(i) * 548390.0 + rng_float * 432.0));

        vec3 q = p;
        q.xz += vec2(2.0 * (i_rng - 0.5)) + grid_frequency * 0.5 * (rng - 0.5) + vec2(cos(camera.time * 0.5), sin(camera.time * 0.5));

        float t = 0.5 * i_rng + (camera.time + 4.0 * music_data.cumulative.x) * (0.5 + 0.25 * fract(i_rng * 54783.0)) + fract(rng_float * 24398.0);
        float bubble_height = fract(t * 0.25);    
        vec3 color = mix(vec3(0.2, 1.0, 2.0), vec3(3.0, 0.2, 1.0), bubble_height);
        float d = sdSphere(q - vec3(0, 20 * ( 2 * bubble_height - 1), 0), 5.0 * (1 - bubble_height));

        result = smoothUnion(result, sdfResult(d, 2.0 * color), 2.0);
    }
    return result;
}

SDFResult sdSynthWaveScene(in vec3 p)
{
    float terrain_bounds = sdSphere(p - vec3(camera.position.x, 0, camera.position.z), 4500.0);
    float terrain_density = sdTerrainCurved(p  * vec3(0.5, 1.0, 1.0)); 
    // float terrain_density = sdPlane(p, vec3(0, 1, 0), 0.0); 
    SDFResult terrain = sdfResult(smoothIntersection(terrain_density, terrain_bounds, 1.0), vec3(0.01, 0.01, 0.02));   
    terrain.reflectivity = 1.0;

    float grid_frequency = 25.0f;
    vec3 grid_p = mod(p + vec3(0, 0,  - 5.0 * camera.time), grid_frequency);    
    float grid_width = 0.0025f + music_data.current.x;
    
    float grid_density = abs(grid_p.x - grid_frequency * 0.5) - grid_width;
    grid_density = min(grid_density, abs(grid_p.z - grid_frequency * 0.5) - grid_width);
    
    SDFResult result = smoothDifference(terrain, sdfResult(grid_density, vec3(0.2, 1.0, 2.0) * (0.8 + 0.4 * music_data.current.x)), 0.5);
    // SDFResult result = sdfResult(sdPlane(p, vec3(0,1,0), 0), vec3(0.2, 1.0, 2.0));
    // result.reflectivity = 1.0;    

    SDFResult sun = sdSunScene((p - camera.position.xyz + vec3(0, -25, 500)) * vec3(0.5, 0.5, 1.0));
    result = smoothUnion(result, sun, 2.0);

    SDFResult bubbles = sdBubblesScene(p);
    result = smoothUnion(result, bubbles, 2.0);
    return result;
}

// ----------------------- Spectrum Scene -----------------------
SDFResult sdSpectrumScene(in vec3 p) {
    float numBars = float(music_data.spectrum_count);
    float barSpacing = 2.0;
    float barWidth = 1.0;
    float gapBuffer = 0.1;
    
    SDFResult result = sdfResult(1e20, vec3(0)); 
    for (int i = -1; i < 2; i++) {
        float cellIndex = clamp(round(p.z / barSpacing) + i, 0.0, numBars - 1.0);
        float localZ = p.z - cellIndex * barSpacing;
        int bin = int(cellIndex);
        float spectrum_value = music_data.spectrum[bin];
        float h = 1.0 + 2.0 * spectrum_value * 25.0;
        vec3 q = vec3(p.x, p.y, localZ);
        float d = sdBox(q - vec3(0, h * 0.5, 0), vec3(barWidth - gapBuffer, h * 0.5, barWidth - gapBuffer)) - 0.125;
        vec3 col = hsv2rgb(2.0 * vec3(0.5 * cellIndex / numBars, 1.0, 0.5));

        result = smoothUnion(result, sdfResult(d, col), 0.0);
    }

    return result;
}

// ----------------------- Demo Scene -----------------------
SDFResult sdDemoScene(in vec3 p) {

    p.z -= 20;

    vec3 q;
    //cube
    q = p + vec3(0, 2.0 * sin(camera.time + 57.0), 0);
    SDFResult cube = sdfResult(sdBox(q, vec3(5)), vec3(1,.2,.2));

     // sphere
    q = p + vec3(20, 2.0 * sin(camera.time + 547.0), 0);
    SDFResult sphere = sdfResult(sdSphere(q, 5), vec3(.2,1,.2));
    
     //torus
    q = p + vec3(-20, 2.0 * sin(camera.time + 94.0), 0);

    q = rotatePoint(q, vec3(
        sin(camera.time * 0.5) * 90.0,
        sin(camera.time * 0.3) * 60,
        sin(camera.time * 0.7) * 90
    ));

    // q = rotatePoint(q, vec3(0, 0, 90));


    SDFResult torus = sdfResult(sdTorus(q, vec2(5, 2)), vec3(.2,.2,1));
    
    // combine
    SDFResult grid = sdfResult(sdPlane(p, vec3(0,1,0), 0), gridColoring(p, vec3(0.7), vec3(0.9)));

    SDFResult result = smoothUnion(cube, sphere, 2.0f);
    result = smoothUnion(result, torus, 2.0f);
    result = smoothUnion(result, grid, 2.0f);
    return result;
}


// ----------------------- sdScene : function used in raymarching -----------------------

SDFResult sdScene(in vec3 p) { 
    
#if SCENE_ID == 1
    return sdSynthWaveScene(p);
#elif SCENE_ID == 2
    return sdfResult(sdMengerScene(p), vec3(0.5));
#elif SCENE_ID == 3
    return sdDemoScene(p);
#elif SCENE_ID == 4
    return sdSpectrumScene(p);
#endif
    return smoothUnion(sdfResult(sdBox(p + vec3(0,10,0), vec3(10)), vec3(0.5)), sdfResult(sdSphere(p, 10.0), vec3(1.0, 0.5, 1.0)), 2.0);
}

#endif // SCENE_GLSL