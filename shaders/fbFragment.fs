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
vec4 InvertColor(vec4 startColor){
    vec4 newColor = 1 - startColor;
    newColor.a = 1;
    return newColor;
}
vec4 GreyScale(vec4 startColor){
    float average = (startColor.r + startColor.g + startColor.b) / 3.0;
    vec4 newColor = vec4(vec3(average), 1);
    return newColor;
}
vec4 ApplyColorEffectsUpTo(vec2 sampleUV, int maxStep) {
    vec4 diffuse = texture(scene, sampleUV);
    vec4 color = vec4(diffuse.rgb, 1.0);

    for (int i = 0; i < maxStep; i++) {
        switch (effects[i]) {
            case 1: color = AddColor(color); break;
            case 2: color = MultColor(color); break;
            case 3: color = InvertColor(color); break;
            case 4: color = GreyScale(color); break;
            default: break;
        }
    }
    return color;
}
vec4 EdgeDetection(float offset, float weight, int currentStep) {
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
        vec4 neighborColor = ApplyColorEffectsUpTo(uv + offsets[i], currentStep);
        col += neighborColor * kernel[i];
    }
    col.a = 1.0;
    return col;
}
vec4 Blur(float offset, int currentStep) {
    vec2 offsets[9] = vec2[](
    vec2(-offset, offset), vec2(0.0f, offset), vec2(offset, offset),
    vec2(-offset, 0.0f), vec2(0.0f, 0.0f), vec2(offset, 0.0f),
    vec2(-offset, -offset), vec2(0.0f, -offset), vec2(offset, -offset)
    );
    float divisor = 16;
    float kernel[9] = float[](
    1.0 / divisor, 2.0 / divisor, 1.0 / divisor,
    2.0 / divisor, 4.0 / divisor, 2.0 / divisor,
    1.0 / divisor, 2.0 / divisor, 1.0 / divisor
    );
    vec4 sampleTex[9];
    vec4 col = vec4(0.0);
    for (int i = 0; i < 9; i++)
    {
        vec4 neighborColor = ApplyColorEffectsUpTo(uv + offsets[i], currentStep);
        col += neighborColor * kernel[i];
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
            color = GreyScale(color);
            break;
        case 5:
            color = EdgeDetection(0.001, -8.0, i);
            break;
        case 6:
            color = Blur(0.001, i);
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