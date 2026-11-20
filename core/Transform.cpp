//
// Created by jaron on 03/11/2026.
//

#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

namespace core {
    Transform::Transform(): translateMatrix(1), rotationQuat(1.0f,0.0f,0.0f,0.0f), rotationAroundQuat(1.0f,0.0f,0.0f,0.0f),
    pivotOffset(0.0f), scaleMatrix(1), position(0.0f),
    rotation(0.0f), rotationAround(0.0f), scale(1.0f) {}

    Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale): Transform()
    {
        // printf("using pos transform and set to (%f,%f,%f)\n", position.x, position.y, position.z);
        Translate(position);
        Scale(scale);
        Rotate(rotation);
    }

    void Transform::Translate(glm::vec3 translation) {
        position += translation;
        translateMatrix = glm::translate(glm::mat4(1.0f), position);
        // printf("pos is now (%f,%f,%f)\n", position.x, position.y, position.z);
    }
    void Transform::Rotate(const glm::vec3 rotationInDegrees)
    {
        rotation += rotationInDegrees;
        //
        // glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        // glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        // glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::quat deltaQuat = glm::quat(glm::radians(rotationInDegrees));
        rotationQuat = glm::normalize(rotationQuat * deltaQuat);
    }

    void Transform::RotateAround(glm::vec3 rotationInDegrees, glm::vec3 offset)
    {
        rotationAround += rotationInDegrees;
        pivotOffset = offset;

        glm::quat deltaQuat = glm::quat(glm::radians(rotationInDegrees));
        rotationAroundQuat = glm::normalize(rotationAroundQuat * deltaQuat);
    }

    void Transform::Scale(glm::vec3 scaleFactor) {
        scale *= scaleFactor;
        scaleMatrix = glm::scale(glm::mat4(1.0f), this->scale);

    }
    void Transform::SetPosition(glm::vec3 position)
    {
        translateMatrix[3][0] = position.x;
        translateMatrix[3][1] = position.y;
        translateMatrix[3][2] = position.z;
    }
    void Transform::SetRotation(glm::vec3 rotationInDegrees)
    {
        rotation = rotationInDegrees;
        rotationQuat = glm::normalize(glm::quat(glm::radians(rotationInDegrees)));
    }

    glm::mat4 Transform::getTransformMatrix() const {
        glm::mat4 offsetMatrix = glm::translate(glm::mat4(1.0f), pivotOffset);

        return translateMatrix
        * glm::mat4_cast(rotationAroundQuat)
        * offsetMatrix
        * glm::mat4_cast(rotationQuat)
        * scaleMatrix;
    }

    glm::vec3 Transform::getPosition() const
    {
        glm::mat4 combinedMat = getTransformMatrix();
        // printf("pos received = (%f,%f,%f)\n", combinedMat[3][0], combinedMat[3][1], combinedMat[3][2]);
        return glm::vec3(combinedMat[3]);
    }

    glm::vec3 Transform::getRotation() const
    {
        return this->rotation;
    }
}