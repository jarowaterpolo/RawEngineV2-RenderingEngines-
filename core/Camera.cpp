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
void Camera::LookAt()
{
    view = glm::lookAt(position + moveAxis,target + moveAxis, up);
    projection = glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
}
void Camera::RotateOnX(float angleDegrees)
{
    glm::vec3 offset = position - target;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), right);
    offset = glm::vec3(rotation * glm::vec4(offset, 1.0f));

    position = target + offset;
    direction = glm::normalize(position - target);
}
void Camera::RotateOnY(float angleDegrees)
{
    glm::vec3 offset = position - target;
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), up);
    offset = glm::vec3(rotation * glm::vec4(offset, 1.0f));

    position = target + offset;
    direction = glm::normalize(position - target);
}
void Camera::UpdateAspectRatio(float aspectRatio)
{
    this->aspectRatio = aspectRatio;
    projection = glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
}
void Camera::Input(GLFWwindow *window, float standardInputValue)
{
    float currentInput = standardInputValue * cameraMoveSpeed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveAxis.z -= currentInput;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveAxis.z += currentInput;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveAxis.x -= currentInput;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveAxis.x += currentInput;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        moveAxis.y += currentInput;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        moveAxis.y -= currentInput;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraMoveSpeed = 2.5f;
    else {
        cameraMoveSpeed = 1.0f;
    }
    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

}