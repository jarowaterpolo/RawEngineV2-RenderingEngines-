//
// Created by jaron on 20/11/2026.
//

#include "Material.h"

namespace core {
    glm::vec4 Material::GetColor() const
    {
        return color;
    }
    float Material::GetSpecularIntensity() const
    {
        return specularIntensity;
    }
    void Material::SetColor(glm::vec4 color)
    {
        printf("Old color (%f,%f,%f) -> new Color (%f,%f,%f) \n", this->color.r, this->color.g, this->color.b, color.r, color.g, color.b);
        this->color = color;
    }
    void Material::SetSpecularIntensity(float specularIntensity)
    {
        this->specularIntensity = specularIntensity;
    }
}
