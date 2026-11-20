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
    void modelRenderer::DrawModel(Model& model, glm::mat4 offsetMatrix, float specularIntensity)
    {
        //color
        uniformColorLocation = glGetUniformLocation(modelShaderProgram, "uniformColor");
        //mvp
        mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
        mMatrixUniform = glGetUniformLocation(modelShaderProgram, "mMatrix");
        //lighting
        light0colorUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].color");
        light0positionUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].position");
        light0ambientIntensityUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].ambientIntensity");
        light0cameraPositionUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].cameraPos");
        light0specularIntensityUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].specularIntensity");

        glm::vec4 baseColor = model.material.GetColor();
        glUseProgram(modelShaderProgram);
        glUniform4f(uniformColorLocation, baseColor.r, baseColor.g, baseColor.b, 1);
        //light
        if (light0colorUniformLocation != -1) {
            glUniform4fv(light0colorUniformLocation, 1, glm::value_ptr(lightObject->GetColor()));
            // printf("lightObject color = (%f,%f,%f,%f) \n", lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
            // printf("%i set to color (%f,%f,%f,%f)\n", light0colorUniformLocation, lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
            // printf("light pos = (%f,%f,%f) \n", lightObject->GetPos().x, lightObject->GetPos().y, lightObject->GetPos().z);
            glUniform3fv(light0positionUniformLocation, 1, glm::value_ptr(lightObject->GetPos()));
            glUniform1f(light0ambientIntensityUniformLocation, 0.2f);
            glUniform3fv(light0cameraPositionUniformLocation, 1, glm::value_ptr(camera->GetPos()));
            glUniform1f(light0specularIntensityUniformLocation, specularIntensity);
        }

        glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, glm::value_ptr(model.transform.getTransformMatrix() * offsetMatrix));
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix(aspectRatio) * camera->GetViewMatrix() * model.transform.getTransformMatrix() * offsetMatrix));
        model.render(*drawMode);
        glBindVertexArray(0);
    }
}