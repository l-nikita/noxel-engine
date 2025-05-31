#pragma once

#include <vector>

#include "mesh.hpp"
#include "mesh_data.hpp"
#include "material.hpp"
#include "transformable.hpp"

class Model : public Transformable
{
protected:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;

public:
    Model(const std::vector<std::shared_ptr<Mesh>>& meshes, const std::vector<std::shared_ptr<Material>>& materials);
    Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);

    void SetMaterial(std::shared_ptr<Material> material, size_t index = 0);

    std::shared_ptr<Material> GetMaterial(size_t index) const;
    std::shared_ptr<Material> GetMaterial() const;
    std::shared_ptr<Mesh> GetMesh(size_t index) const;

    void Draw();
    void Dispose();
};