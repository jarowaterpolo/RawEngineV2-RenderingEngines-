#version 400 core
out vec4 FragColor;
in vec3 fNor;
in vec2 uv;
uniform sampler2D scene;
uniform vec4 additionColor;
uniform vec4 multColor;
uniform ivec4 effects;
int steps = 4;

vec4 AddColor(vec4 startColor){
    return startColor + additionColor;
}
vec4 MultColor(vec4 startColor){
    return startColor * multColor;
}
vec4 InvertColor(vec4 color){
    vec4 newColor = 1 - color;
    newColor.a = 1;
    return newColor;
}
vec4 EdgeDetection(float offset, float weight) {
    vec2 offsets[9] = vec2[](
    vec2(-offset, offset), vec2(0.0f, offset), vec2(offset, offset),
    vec2(-offset, 0.0f), vec2(0.0f, 0.0f), vec2(offset, 0.0f),
    vec2(-offset, -offset), vec2(0.0f, -offset), vec2(offset, -offset)
    );
    float kernel[9] = float[](
    1.0, 1.0, 1.0,
    1.0, weight, 1.0,
    1.0, 1.0, 1.0
    );
    vec4 sampleTex[9];
    vec4 col = vec4(0.0);
    for (int i = 0; i < 9; i++)
    {
        col += texture(scene, uv.st + offsets[i]) * kernel[i];
    }
    col.a = 1.0;
    return col;
}
vec4 DoEffect(vec4 color, int i){
    switch(effects[i]){
        case 1:
            color = AddColor(color);
            break;
        case 2:
            color = MultColor(color);
            break;
        case 3:
            color = InvertColor(color);
            break;
        case 4:
            color = EdgeDetection(0.001, -8.0);
            break;

        default:
        break;
    }
    return color;
}
void main()
{
    vec4 diffuse = texture(scene, uv);
    vec4 baseSceneColor = vec4(diffuse.r, diffuse.g, diffuse.b, 1);

    for (int i = 0; i < steps; i++) {
        baseSceneColor = DoEffect(baseSceneColor, i);
    }

    FragColor = baseSceneColor;
}