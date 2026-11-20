//
// Created by jaron on 03/11/2026.
//

#ifndef RAWENGINE_TRANSFORM_H
#define RAWENGINE_TRANSFORM_H
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>

namespace core {
    class Transform {
    private:
        glm::mat4 translateMatrix;
        glm::quat rotationQuat;
        glm::quat rotationAroundQuat;
        glm::mat4 scaleMatrix;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 rotationAround;
        glm::vec3 pivotOffset;
    public:
        Transform();
        Transform(glm::vec3 positon, glm::vec3 rotation, glm::vec3 scale);

        glm::vec3 scale;


        void Translate(glm::vec3 translation);
        void Rotate(glm::vec3 rotationInDegrees);
        void RotateAround(glm::vec3 rotationInDegrees, glm::vec3 offset);
        void Scale(glm::vec3 scale);
        void SetPosition(glm::vec3 position);
        void SetRotation(glm::vec3 rotationInDegrees);
        glm::mat4 getTransformMatrix() const;
        glm::vec3 getPosition() const;
        glm::vec3 getRotation() const;
    };
}

#endif //RAWENGINE_TRANSFORM_H
