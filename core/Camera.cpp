//
// Created by jaron on 09/10/2026.
//

#include "Camera.h"

#include <ios>

namespace core {
    Camera::Camera(float aspectRatio)
    {
        //base
        transform = Transform(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f), glm::vec3(1.0f));
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
    glm::vec3 Camera::GetPos()
    {
        return transform.getPosition();
    }
    glm::vec3 Camera::GetTarget() const
    {
        return transform.getPosition() + GetForward();
    }
    glm::vec3 Camera::GetUp()
    {
        return up;
    }
    glm::vec3 Camera::GetForward() const
    {
        return glm::normalize(transform.getTransformMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    }
    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(transform.getPosition(),GetTarget(), up);
    }
    glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
    {
        return glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
    }
    void Camera::UpdateCameraVectors()
    {
        glm::mat4 tM = transform.getTransformMatrix();

        direction = glm::normalize(tM * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
        right = glm::normalize(glm::cross(direction, worldUp));
        up = glm::normalize(glm::cross(right, direction));

        target = transform.getPosition() + direction;
    }
    void Camera::RotateOnX(float angleDegrees)
    {
        float newPitch = totalRotationOnX + angleDegrees;
        if (newPitch > 89.0f || newPitch < -89.0f) {
            return;
        }
        totalRotationOnX = newPitch;

        transform.Rotate(right, glm::radians(angleDegrees));

        UpdateCameraVectors();
    }
    void Camera::RotateOnY(float angleDegrees)
    {
        float newPitch = totalRotationOnY + angleDegrees;
        if (newPitch > 179.0f || newPitch < -179.0f) {
            return;
        }
        totalRotationOnY = newPitch;

        transform.Rotate(up, glm::radians(angleDegrees));

        UpdateCameraVectors();
    }
    void Camera::Input(GLFWwindow *window, float deltaTime)
    {
        float speed = cameraMoveSpeed * cameraSpeedUpMult * deltaTime;
        glm::vec3 translation(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            translation += direction * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            translation -= direction * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            translation -= right * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            translation += right * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            translation += up * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            translation -= up * speed;
        }

        if (translation != glm::vec3(0.0f)) {
            transform.Translate(translation);
            target += translation;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            if (FOV > 30) {
                FOV -= .05f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            if (FOV < 120) {
                FOV += .05f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cameraSpeedUpMult = 2.5f;
        }
        else {
            cameraSpeedUpMult = 1.0f;
        }
    }
}

