#version 400 core
out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;
uniform float colorMult;
uniform vec4 uniformColor;

struct Light{
    vec4 color;
    vec3 position;
    float ambientIntensity;
    vec3 cameraPos;
    float specularIntensity;
};

uniform Light lights[20];
//uniform int lightsAmount;

void main()
{
    //a
    vec4 ambientColor = lights[0].ambientIntensity * lights[0].color * uniformColor;
    ambientColor.a = 1.0f;
    //d
    vec3 direction = fPos - lights[0].position;
    direction = normalize(direction);
    vec3 normal = normalize(fNor);
    vec4 diffuseColor = max(dot(direction, -normal), 0.0f) * lights[0].color * uniformColor;
    //s
    vec3 reflectedRay = reflect(-direction, normal);
    vec3 view = normalize(fPos - lights[0].cameraPos);
    vec4 sCol = vec4(1,1,1,1);
//    vec4 specularColor = pow(max(dot(reflectedRay, view), 0.0f), lights[0].specularIntensity) * lights[0].color * uniformColor;
    vec4 specularColor = pow(max(dot(reflectedRay, view), 0.0f), lights[0].specularIntensity) * lights[0].color;

    FragColor = ambientColor + diffuseColor + specularColor;
//    FragColor = lights[0].color;
//    FragColor = lights[0].position;
//    FragColor = ambientColor;
//    FragColor = diffuseColor;
//    FragColor = uniformColor + specularColor;

//   FragColor = vec4(fNor.x, fNor.y, fNor.z, 1);
//   FragColor = uniformColor;
//   FragColor = vec4(fNor.x, fNor.y, fNor.z, 1) + uniformColor;
//   FragColor = vec4(fPos.x, fPos.y, fPos.z, 1);
//   FragColor = texture(, uv);


//   FragColor = vec4(fNor.x, fNor.y, fNor.z, 1) * colorMult;
//FragColor = vec4(fNor.x, fNor.y, fNor.z, 1) * uniformColor;
}