//
// Created by jaron on 20/11/2026.
//

#ifndef RAWENGINE_MATERIAL_H
#define RAWENGINE_MATERIAL_H
#include <glm/glm.hpp>

namespace core {
    class Material {
    protected:
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        float specularIntensity = 1.0f;
    public:
        glm::vec4 GetColor() const;
        float GetSpecularIntensity() const;
        void SetColor(glm::vec4 color);
        void SetSpecularIntensity(float specularIntensity);
    };
}

#endif //RAWENGINE_MATERIAL_H
