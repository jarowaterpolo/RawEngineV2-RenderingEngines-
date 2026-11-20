//
// Created by jaron on 09/10/2026.
//

#include "Camera.h"

namespace core {
    Camera::Camera(float aspectRatio)
    {
        //base
        transform = Transform(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        target = glm::vec3(0.0f, 0.0f, 0.0f);
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        UpdateCameraVectors();
        //extra
        FOV = 45.0f;
        clipNear = 0.1f;
        clipFar = 100.0f;
        this->aspectRatio = aspectRatio;
        cameraMoveSpeed = 10.0f;
    }
    Camera::Camera(float aspectRatio, glm::vec3 cameraPos, glm::vec3 cameraTarget, float FOV, float clipNear, float clipFar)
    {
        this->transform = Transform(cameraPos, glm::vec3(0), glm::vec3(1));
        this->target = cameraTarget;
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
         UpdateCameraVectors();
        //extra
        this->aspectRatio = aspectRatio;
        this->FOV = FOV;
        this->clipNear = clipNear;
        this->clipFar = clipFar;
        cameraMoveSpeed = 10.0f;
    }
    glm::vec3 Camera::GetPos() const
    {
        return transform.getPosition();
    }
    glm::vec3 Camera::GetTarget() const
    {
        return transform.getPosition() + GetForward();
    }
    glm::vec3 Camera::GetForward() const
    {
        return glm::normalize(transform.getTransformMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    }
    glm::vec3 Camera::GetRight() const
    {
        return right;
    }
    glm::vec3 Camera::GetUp() const
    {
        return up;
    }
    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(transform.getPosition(),transform.getPosition() + forward, up);
    }
    glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
    {
        return glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
    }
    void Camera::UpdateCameraVectors()
    {
        glm::mat4 tM = transform.getTransformMatrix();

        forward = glm::normalize(tM * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        right = glm::normalize(glm::cross(forward, worldUp));
        up = glm::normalize(glm::cross(right, forward));

        target = transform.getPosition() + forward;
    }
    void Camera::Rotate(glm::vec3 angleDegrees)
    {
        // printf("angle degrees is (%f,%f,%f) \n", angleDegrees.x, angleDegrees.y, angleDegrees.z);
        float newPitchX = totalRotationOnX + angleDegrees.x;

        if (newPitchX > 89.0f || newPitchX < -89.0f) {
            return;
        }
        totalRotationOnX = newPitchX;

        float newPitchY = totalRotationOnY + angleDegrees.y;
        if (newPitchY > 179.0f || newPitchY < -179.0f) {
            return;
        }
        totalRotationOnY = newPitchY;

        // transform.OldRotate(right, glm::radians(angleDegrees));
        // transform.Rotate(glm::vec3(angleDegrees, 0.0f, 0.0f));
        transform.SetRotation(glm::vec3(totalRotationOnX, totalRotationOnY, 0.0f));
        // printf("rotation is (%f,%f,%f) \n", transform.getRotation().x, transform.getRotation().y, transform.getRotation().z);

        UpdateCameraVectors();
    }
}