#pragma once

#include <vector>
#include <memory>

#include "imesh.hpp"
#include "imaterial.hpp"
#include "common/transformable.hpp"

class Model : public Transformable
{
protected:
    std::vector<IMesh*> m_meshes;
    std::vector<IMaterial*> m_materials;

public:
    Model(const std::vector<IMesh*>& meshes, const std::vector<IMaterial*>& materials);
    Model(IMesh* mesh, IMaterial* material);
    ~Model() { Destory(); }

    void SetMaterial(IMaterial* material, size_t index = 0);

    IMaterial* GetMaterial(size_t index) const;
    IMaterial* GetMaterial() const;
    IMesh* GetMesh(size_t index) const;

    void Draw();
    void Destory();
};