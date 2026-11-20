//
// Created by jaron on 03/11/2026.
//

#ifndef RAWENGINE_GAMEOBJECT_H
#define RAWENGINE_GAMEOBJECT_H
#include <string>
#include "Transform.h"
#include "Material.h"

namespace core {
    class Object {
    protected:
        std::string name;
    public:
        Transform transform;
        Material material;

        void SetName(std::string name);
        std::string GetName()
        {
            return name;
        }
    };
}


#endif //RAWENGINE_GAMEOBJECT_H
