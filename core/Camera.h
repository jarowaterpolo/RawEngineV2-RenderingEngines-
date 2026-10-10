//
// Created by jaron on 09/10/2026.
//

#ifndef RAWENGINE_CAMERA_H
#define RAWENGINE_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


class Camera {
    //base
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = glm::normalize(position - target);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(worldUp, direction));
    glm::vec3 up = glm::cross(direction, right);
    //extra
    float cameraMoveSpeed;
    float FOV;
    float aspectRatio;
    float clipNear;
    float clipFar;
    //matrices
    glm::mat4 view;
    glm::mat4 projection = glm::perspective(glm::radians(FOV), aspectRatio, clipNear, clipFar);
public:
    glm::vec3 moveAxis;
    Camera(float aspectRatio);
    Camera(float aspectRatio, glm::vec3 cameraPos, glm::vec3 cameraTarget, float FOV, float clipNear, float clipFar);

    glm::vec3 GetPos();
    glm::vec3 GetTarget();
    glm::vec3 GetUp();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    void Input(GLFWwindow *window, float standardInputValue);
    void LookAt();
    void RotateOnX(float angleDegrees);
    void RotateOnY(float angleDegrees);
    void UpdateAspectRatio(float aspectRatio);
};

#endif //RAWENGINE_CAMERA_H
