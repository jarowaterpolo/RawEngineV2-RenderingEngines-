#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBit;
uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
out vec3 fPos;
out vec3 fNor;
out vec2 uv;
out mat3 TBN;

void main() {
  fPos = vec3(mMatrix * vec4(aPos, 1.0));
  fNor = vec3(mMatrix * vec4(aNor,0.0));
  uv = aUv;

  TBN = transpose(mat3(
          vec3(mMatrix * vec4(aTan, 0.0)),
          vec3(mMatrix * vec4(aBit, 0.0)),
          vec3(mMatrix * vec4(aNor, 0.0))
                  ));

  gl_Position = mvpMatrix * vec4(aPos, 1.0);
}