//
// Created by jaron on 15/11/2026.
//

#ifndef RAWENGINE_MODELRENDERER_H
#define RAWENGINE_MODELRENDERER_H
#include "Camera.h"
#include "LightObject.h"
#include "model.h"

namespace core {
    class modelRenderer {
        const unsigned int modelShaderProgram;
        LightObject lightObject;
        glm::vec3 lightPos;
        Camera camera;
        float aspectRatio;
        GLenum drawMode;


        public:
            void DrawModel(Model model, glm::vec4 baseColor, glm::mat4 offsetMatrix, GLint uniformColorLocation,
                GLint light0colorUniformLocation, GLint light0positionUniformLocation, GLint light0ambientIntensityUniformLocation,
                GLint light0cameraPositionUniformLocation, GLint light0specularIntensityUniformLocation, float specularIntensity, GLint mMatrixUniform, GLint mvpMatrixUniform);
    };
}

#endif //RAWENGINE_MODELRENDERER_H
