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
        private:
            unsigned int modelShaderProgram;
            LightObject* lightObject;
            Camera* camera;
            GLenum* drawMode;
            float aspectRatio;

            //color
            GLint uniformColorLocation;
            //mvp
            GLint mvpMatrixUniform;
            GLint mMatrixUniform;
            //lighting
            GLint light0colorUniformLocation;
            GLint light0positionUniformLocation;
            GLint light0ambientIntensityUniformLocation;
            GLint light0cameraPositionUniformLocation;
            GLint light0specularIntensityUniformLocation;

        public:
            modelRenderer(unsigned int modelShaderProgram, LightObject* lightObject, Camera* camera, float aspectRatio, GLenum* drawMode);
            void SetAspectRatio(float aspectRatio);
            void SetDrawMode(GLenum* drawMode);
            void SetModelShaderProgram(unsigned int modelShaderProgram);
            void SetCamera(Camera camera);
            void UpdateLightObject(LightObject* lightObject);
            void DrawModel(Model& model, glm::mat4 offsetMatrix, float specularIntensity);
    };
}

#endif //RAWENGINE_MODELRENDERER_H
