//
// Created by jaron on 23/10/2026.
//

#include "LightObject.h"

LightObject::LightObject(glm::vec3 position, glm::vec3 color)
{
    this->position = position;
    this->color = glm::vec4(color, 1.0f);
    printf("position set to (%f,%f,%f)\n", position.x, position.y, position.z);
    printf("color set to (%f,%f,%f,%f)\n", color.r, color.g, color.b, 1.0f);

}
LightObject::~LightObject() = default;
LightObject& LightObject::operator=(const LightObject& anotherLightObject)
{
    if (this == &anotherLightObject) {
        this->position = anotherLightObject.position;
        this->color = anotherLightObject.color;
    }
    return *this;
}
glm::vec3 LightObject::GetPos() const
{
    return this->position;
}
glm::vec4 LightObject::GetColor() const
{
    return this->color;
}
