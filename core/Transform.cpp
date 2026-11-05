//
// Created by jaron on 03/11/2026.
//

#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

namespace core {
    Transform::Transform(): transformMatrix(1)
    {
        this->position = glm::vec3(0.0f, 0.0f, 0.0f);
        this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    Transform::Transform(glm::vec3 positon, glm::vec3 rotation, glm::vec3 scale): transformMatrix(1)
    {
        this->position = positon;
        this->rotation = rotation;
        this->scale = scale;

        transformMatrix = glm::mat4(1.0f);
        Translate(positon);
        Scale(scale);
        Rotate(glm::vec3(0.0f,0.0f, 1.0f), glm::radians(rotation.z));
        Rotate(glm::vec3(0.0f,1.0f, 0.0f), glm::radians(rotation.y));
        Rotate(glm::vec3(1.0f,0.0f, 0.0f), glm::radians(rotation.x));
    }

    void Transform::Translate(glm::vec3 translation) {
        transformMatrix[3] += glm::vec4(translation, 0.0f);
        position = glm::vec3(transformMatrix[3]);
    }

    void Transform::Rotate(glm::vec3 axis, float radians) {
        transformMatrix = glm::rotate(transformMatrix, radians, axis);

        if (axis == glm::vec3(0.0f, 0.0f, 1.0f)) {
                rotation.z += glm::degrees(radians);
        }
        else if (axis == glm::vec3(0.0f, 1.0f, 0.0f)) {
            rotation.y += glm::degrees(radians);
        }
        else if (axis == glm::vec3(1.0f, 0.0f, 0.0f)) {
            rotation.x += glm::degrees(radians);
        }
    }

    void Transform::Scale(glm::vec3 scale) {
        transformMatrix = glm::scale(transformMatrix, scale);

        this->scale = glm::vec3(transformMatrix[0][0], transformMatrix[1][1], transformMatrix[2][2]);
    }

    glm::mat4 Transform::getTransformMatrix() const {
        return this->transformMatrix;
    }

    glm::vec3 Transform::getPosition() const
    {
        return glm::vec3(transformMatrix[3]);
    }
}