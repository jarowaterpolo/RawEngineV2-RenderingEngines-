//
// Created by jaron on 09/10/2026.
//

#include "Camera.h"

Camera::Camera(float aspectRatio)
{
    //base
    position = glm::vec3(0.0f, 0.0f, 10.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    direction = glm::normalize(position - target);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::normalize(glm::cross(worldUp, direction));
    up = glm::cross(direction, right);
    //extra
    FOV = 45.0f;
    clipNear = 0.1f;
    clipFar = 100.0f;
    this->aspectRatio = aspectRatio;
    cameraMoveSpeed = 10.0f;
}
Camera::Camera(float aspectRatio, glm::vec3 cameraPos, glm::vec3 cameraTarget, float FOV, float clipNear, float clipFar)
{
    this->position = cameraPos;
    this->target = cameraTarget;
    direction = glm::normalize(cameraPos - cameraTarget);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::normalize(glm::cross(worldUp, direction));
    up = glm::cross(direction, right);
    //extra
    this->aspectRatio = aspectRatio;
    this->FOV = FOV;
    this->clipNear = clipNear;
    this->clipFar = clipFar;
    cameraMoveSpeed = 20.0f;
}
glm::vec3 Camera::GetPos()
{
    return position;
}
glm::vec3 Camera::GetTarget()
{
    return target;
}
glm::vec3 Camera::GetUp()
{
    return up;
}
glm::mat4 Camera::GetViewMatrix()
{
    return view;
}
glm::mat4 Camera::GetProjectionMatrix()
{
    return projection;
}
void Camera::UpdateAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
    projection = glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
}
void Camera::UpdateCameraVectors()
{
    direction = glm::normalize(target - position);
    right = glm::normalize(glm::cross(direction, worldUp));
    up = glm::normalize(glm::cross(right, direction));
}
void Camera::LookAt()
{
    view = glm::lookAt(position,target, up);
    projection = glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
}
void Camera::RotateOnX(float angleDegrees)
{
    float newPitch = totalRotationOnX + angleDegrees;
    if (newPitch > 89.0f || newPitch < -89.0f) {
        return;
    }
    totalRotationOnX = newPitch;

    glm::vec3 offset = position - target;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), right);
    offset = glm::vec3(rotation * glm::vec4(offset, 1.0f));

    position = target + offset;

    UpdateCameraVectors();
}
void Camera::RotateOnY(float angleDegrees)
{
    float newPitch = totalRotationOnY + angleDegrees;
    if (newPitch > 179.0f || newPitch < -179.0f) {
        return;
    }
    totalRotationOnY = newPitch;

    glm::vec3 offset = position - target;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), up);
    offset = glm::vec3(rotation * glm::vec4(offset, 1.0f));

    position = target + offset;

    UpdateCameraVectors();
}
void Camera::Input(GLFWwindow *window, float deltaTime)
{
    float currentInput = cameraMoveSpeed * cameraSpeedUpMult * deltaTime;
    printf("cameraMoveSpeed = %f, DeltaTime = %f, currentInput = %f \n", cameraMoveSpeed * cameraSpeedUpMult, deltaTime, currentInput);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * currentInput;
        target += direction * currentInput;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * currentInput;
        target -= direction * currentInput;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * currentInput;
        target -= right * currentInput;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * currentInput;
        target += right * currentInput;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position += up * currentInput;
        target += up * currentInput;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position -= up * currentInput;
        target -= up * currentInput;
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

