//
// Created by jaron on 03/11/2026.
//

#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

namespace core {
    Transform::Transform(): transformMatrix(1), translateMatrix(1), rotateMatrix(1), scaleMatrix(1)
    {
        this->position = glm::vec3(0.0f, 0.0f, 0.0f);
        this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    Transform::Transform(glm::vec3 positon, glm::vec3 rotation, glm::vec3 scale): transformMatrix(1), translateMatrix(1), rotateMatrix(1), scaleMatrix(1)
    {
        this->position = positon;
        this->rotation = rotation;
        this->scale = scale;

        Translate(positon);
        Rotate(rotation);
        Scale(scale);
    }

    void Transform::Translate(glm::vec3 translation) {
        translateMatrix = glm::translate(transformMatrix, translation);
    }

    void Transform::Rotate(glm::vec3 rotation) {
        this->rotation = rotation;
        rotateMatrix = glm::rotate(transformMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(transformMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(transformMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0));
    }

    void Transform::Scale(glm::vec3 scale) {
        scaleMatrix = glm::scale(transformMatrix, scale);
    }

    glm::mat4 Transform::getTransformMatrix() const {
        return this->translateMatrix * this->rotateMatrix * this->scaleMatrix;
    }
}