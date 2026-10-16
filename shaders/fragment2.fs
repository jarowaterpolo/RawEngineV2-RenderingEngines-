#version 400 core
out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;
uniform float colorMult;
uniform vec4 uniformColor;

void main()
{
//   FragColor = vec4(fNor.x, fNor.y, fNor.z, 1);
//   FragColor = vec4(fNor.x, fNor.y, fNor.z, 1) * colorMult;
//   FragColor = uniformColor;
//   FragColor = vec4(fNor.x, fNor.y, fNor.z, 1) + uniformColor;
FragColor = vec4(fNor.x, fNor.y, fNor.z, 1) * uniformColor;
//   FragColor = vec4(fPos.x, fPos.y, fPos.z, 1);
//   FragColor = texture(, uv);
}