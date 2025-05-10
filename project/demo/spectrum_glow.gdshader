shader_type canvas_item;

uniform sampler2D screen_texture : hint_screen_texture, repeat_disable, filter_linear;
uniform float glow_strength = 0.5;
uniform float mip_level = 3.0; // Lower mip levels = blurrier samples

void fragment() {
    vec4 original = texture(screen_texture, SCREEN_UV);

    // Sample a lower mip level to create blur
    vec4 blurred = textureLod(screen_texture, SCREEN_UV, 8.0);

    // Additive blending with the original color
    vec4 final_color = original + blurred * glow_strength;

    COLOR = final_color;
}
