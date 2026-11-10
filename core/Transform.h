//
// Created by jaron on 03/11/2026.
//

#ifndef RAWENGINE_TRANSFORM_H
#define RAWENGINE_TRANSFORM_H
#include <glm/fwd.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace core {
    class Transform {
    private:
        glm::mat4 transformMatrix;
        glm::mat4 translateMatrix;
        glm::mat4 rotateMatrix;
        glm::mat4 scaleMatrix;
    public:
        Transform();
        Transform(glm::vec3 positon, glm::vec3 rotation, glm::vec3 scale);

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;


        void Translate(glm::vec3 translation);
        void Rotate(glm::vec3 axis, float radians);
        void Scale(glm::vec3 scale);
        glm::mat4 getTransformMatrix() const;
        glm::vec3 getPosition() const;
    };
}

#endif //RAWENGINE_TRANSFORM_H
