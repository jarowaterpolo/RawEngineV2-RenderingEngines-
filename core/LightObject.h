//
// Created by jaron on 23/10/2026.
//

#ifndef RAWENGINE_LIGHTOBJECT_H
#define RAWENGINE_LIGHTOBJECT_H
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


class LightObject {
private:
        glm::vec3 position{};
        glm::vec4 color{};
public:
        LightObject(glm::vec3 position, glm::vec3 color);
        ~LightObject();
        LightObject& operator=(const LightObject &anotherLightObject);
        glm::vec3 GetPos()const;
        glm::vec4 GetColor()const;
};


#endif //RAWENGINE_LIGHTOBJECT_H
