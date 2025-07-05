#include "iengine.hpp"
#include "model.hpp"
#include "render_camera.hpp"
#include "cmd/cmd.hpp"

Model::Model(const std::vector<IMesh*>& meshes, const std::vector<IMaterial*>& materials)
{
    m_meshes = meshes;
    m_materials.reserve(meshes.size());

    for (auto i = 0; i < m_meshes.size(); ++i)
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

void Model::Draw()
{
    for (auto i = 0; i < m_meshes.size(); ++i)
    {
        auto mat = GetMaterial(i);
        if (!mat)
        {
            CMD_LOG("Material is invalid!", ERROR);
            continue;
        }

        mat->Bind();
        m_meshes[i]->Draw();
        mat->Unbind();
    }
}

void Model::SetMaterial(IMaterial* material, size_t index)
{
    if (!material)
        throw std::invalid_argument("Material is null");
    if (index >= m_materials.size())
        throw std::out_of_range("Invalid material index");

    if (m_materials[index])
    {
        delete m_materials[index];
        m_materials[index] = nullptr;
    }

    m_materials[index] = material;
}

IMaterial* Model::GetMaterial(size_t index) const
{
    return (index < m_materials.size()) ? m_materials[index] : nullptr;
}

IMaterial* Model::GetMaterial() const
{
    return !m_materials.empty() ? m_materials[0] : nullptr;
}

IMesh* Model::GetMesh(size_t index) const
{
    return (index < m_meshes.size()) ? m_meshes[index] : nullptr;
}

void Model::Destory()
{
    for (auto& mesh : m_meshes)
        if (mesh) delete mesh;

    for (auto& mat : m_materials)
        if (mat) delete mat;
}