#pragma once

#include <vector>
#include <memory>

#include "imesh.hpp"
#include "ishader.hpp"
#include "imaterial.hpp"
#include "common/transformable.hpp"

// Model isn't owner of meshes and materials. It just handler which render them.
class Model : public Transformable
{
protected:
    std::vector<IMesh*> m_meshes;
    std::vector<IMaterial*> m_materials;

public:
    Model(const std::vector<IMesh*>& meshes, const std::vector<IMaterial*>& materials);
    Model(IMesh* mesh, IMaterial* material);

    void SetMaterial(IMaterial* material, size_t index = 0);

    IMaterial* GetMaterial() const;
    IMaterial* GetMaterial(size_t index) const;
    std::vector<IMaterial*> GetMaterials() const;

    IMesh* GetMesh() const;
    IMesh* GetMesh(size_t index) const;
    std::vector<IMesh*> GetMeshes() const;

    void Draw();
};