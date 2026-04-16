#include "irenderer.hpp"
#include "ishaderprogram.hpp"
#include "model.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Model::Model(const std::vector<IMesh*>& meshes, const std::vector<IMaterial*>& materials)
{
    m_meshes = meshes;
    m_materials.reserve(meshes.size());

    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        auto mat = (i < materials.size() && materials[i])
            ? materials[i]
            : g_ErrorMaterial;

        m_materials.push_back(mat);
    }
}

Model::Model(IMesh* mesh, IMaterial* material)
{
    m_meshes.push_back(std::move(mesh));

    if (!material)
        material = g_ErrorMaterial;

    m_materials.push_back(std::move(material));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Model::Draw()
{
    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        auto mat = GetMaterial(i);
        if (!mat)
        {
            CMD_ERROR("Material is invalid!", CMD::RENDERING);
            continue;
        }

        mat->GetShader()->SetVSVar("model", m_Transform.GetTransformMatrix());
        mat->Bind();
        g_Renderer->SetPrimitiveTopology(TRIANGLES);
        m_meshes[i]->Draw();
        mat->Unbind();
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Model::SetMaterial(IMaterial* material, size_t index)
{
    assert(material);
    assert(index < m_materials.size());

    if (m_materials[index])
    {
        delete m_materials[index];
        m_materials[index] = nullptr;
    }

    m_materials[index] = material;
}

//-----------------------------------------------------------------------------
IMaterial* Model::GetMaterial() const
{
    return !m_materials.empty() ? m_materials[0] : nullptr;
}

IMaterial* Model::GetMaterial(size_t index) const
{
    return (index < m_materials.size()) ? m_materials[index] : nullptr;
}

std::vector<IMaterial*> Model::GetMaterials() const
{
    return m_materials;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
IMesh* Model::GetMesh() const
{
    return !m_meshes.empty() ? m_meshes[0] : nullptr;
}

IMesh* Model::GetMesh(size_t index) const
{
    return (index < m_meshes.size()) ? m_meshes[index] : nullptr;
}

std::vector<IMesh*> Model::GetMeshes() const
{
    return m_meshes;
}