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

    Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale): transformMatrix(1)
    {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;

        transformMatrix = glm::mat4(1.0f);
        printf("using pos transform and set to (%f,%f,%f)\n", position.x, position.y, position.z);
        Translate(position);
        Scale(scale);
        Rotate(rotation);
    }

    void Transform::Translate(glm::vec3 translation) {
        position += translation;
        translateMatrix = glm::translate(transformMatrix, position);
        printf("pos is now (%f,%f,%f)\n", position.x, position.y, position.z);
    }

    void Transform::OldRotate(glm::vec3 axis, float radians) {
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

    void Transform::Rotate(const glm::vec3 rotationInDegrees)
    {
        rotateMatrix = glm::rotate(transformMatrix, glm::radians(rotationInDegrees.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::rotate(transformMatrix, glm::radians(rotationInDegrees.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(transformMatrix, glm::radians(rotationInDegrees.z), glm::vec3(0.0f, 0.0f, 1.0f));

        rotation += rotationInDegrees;
    }

    void Transform::Scale(glm::vec3 scale) {
        scaleMatrix = glm::scale(transformMatrix, scale);

        this->scale = glm::vec3(transformMatrix[0][0], transformMatrix[1][1], transformMatrix[2][2]);
    }

    glm::mat4 Transform::getTransformMatrix() const {
        return this->translateMatrix*rotateMatrix*scaleMatrix;
    }

    glm::vec3 Transform::getPosition() const
    {
        glm::mat4 combinedMat = getTransformMatrix();
        printf("pos received = (%f,%f,%f)\n", combinedMat[3][0], combinedMat[3][1], combinedMat[3][2]);
        return glm::vec3(combinedMat[3]);
    }
}