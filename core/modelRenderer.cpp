//
// Created by jaron on 15/11/2026.
//

#include "modelRenderer.h"

namespace core {
    modelRenderer::modelRenderer(unsigned int modelShaderProgram, LightObject* lightObject, Camera* camera, float aspectRatio, GLenum* drawMode) : modelShaderProgram(
            modelShaderProgram), lightObject(lightObject), camera(camera)
    {
        this->aspectRatio = aspectRatio;
        this->drawMode = drawMode;
    }

    void modelRenderer::SetModelShaderProgram(unsigned int modelShaderProgram)
    {
        this->modelShaderProgram = modelShaderProgram;
    }

    void modelRenderer::UpdateLightObject(LightObject *lightObject)
    {
        this->lightObject = lightObject;
    }

    //drawModel
    void modelRenderer::DrawModel(Model& model, glm::vec4 baseColor, glm::mat4 offsetMatrix, GLint uniformColorLocation, GLint light0colorUniformLocation,
        GLint light0positionUniformLocation, GLint light0ambientIntensityUniformLocation, GLint light0cameraPositionUniformLocation,
        GLint light0specularIntensityUniformLocation, float specularIntensity, GLint mMatrixUniform, GLint mvpMatrixUniform)
    {
        glUseProgram(modelShaderProgram);
        glUniform4f(uniformColorLocation, baseColor.r, baseColor.g, baseColor.b, 1);
        //light
        if (light0colorUniformLocation != -1) {
            glUniform4fv(light0colorUniformLocation, 1, glm::value_ptr(lightObject->GetColor()));
            // printf("lightObject color = (%f,%f,%f,%f) \n", lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
            // printf("%i set to color (%f,%f,%f,%f)\n", light0colorUniformLocation, lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
            printf("light pos = (%f,%f,%f) \n", lightObject->GetPos().x, lightObject->GetPos().y, lightObject->GetPos().z);
            glUniform3fv(light0positionUniformLocation, 1, glm::value_ptr(lightObject->GetPos()));
            glUniform1f(light0ambientIntensityUniformLocation, 0.2f);
            glUniform3fv(light0cameraPositionUniformLocation, 1, glm::value_ptr(camera->GetPos()));
            glUniform1f(light0specularIntensityUniformLocation, specularIntensity);
        }

        glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, glm::value_ptr(model.getModelMatrix() * offsetMatrix));
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix(aspectRatio) * camera->GetViewMatrix() * model.getModelMatrix() * offsetMatrix));
        model.render(*drawMode);
        glBindVertexArray(0);
    }
}