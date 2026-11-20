#pragma once

#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"
#include "object.h"

namespace core {
    class Model : public Object {
    private:
        std::vector<core::Mesh> meshes;
        glm::mat4 modelMatrix;
    public:
        Model(std::vector<core::Mesh> meshes) : meshes(meshes), modelMatrix(1) {}

        void render(GLenum drawMode);
    };
}