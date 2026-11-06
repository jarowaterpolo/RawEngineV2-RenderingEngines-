//
// Created by jaron on 09/10/2026.
//

#ifndef RAWENGINE_CAMERA_H
#define RAWENGINE_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.h"

namespace core {
    class Camera : public GameObject {
        //base
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f);
        glm::vec3 forward = glm::normalize(position - target);
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
        glm::vec3 up = glm::cross(forward, right);
        //extra
        float cameraMoveSpeed;
        float cameraSpeedUpMult;
        float aspectRatio;
        float clipNear;
        float clipFar;
        float totalRotationOnX;
        float totalRotationOnY;

    public:
        Camera(float aspectRatio);
        Camera(float aspectRatio, glm::vec3 cameraPos, glm::vec3 cameraTarget, float FOV, float clipNear, float clipFar);

        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        float FOV;

        glm::vec3 GetPos();
        glm::vec3 GetTarget() const;
        glm::vec3 GetForward() const;
        glm::vec3 GetRight() const;
        glm::vec3 GetUp() const;
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix(float aspectRatio) const;

        void RotateOnX(float angleDegrees);
        void RotateOnY(float angleDegrees);
        void Translate(glm::vec3 translationDirection, float currentInput);
        void UpdateCameraVectors();
    };
}

#endif //RAWENGINE_CAMERA_H
