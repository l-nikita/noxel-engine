#include "iengine.hpp"
#include "model.hpp"
#include "render_camera.hpp"

Model::Model(const std::vector<std::shared_ptr<Mesh>>& meshes, const std::vector<std::shared_ptr<Material>>& materials)
{
    m_meshes = meshes;
    m_materials.reserve(meshes.size());

    for (auto i = 0; i < m_meshes.size(); ++i)
    {
        auto mat = (i < materials.size() && materials[i])
            ? materials[i]
            : Material::GetError();

        m_materials.push_back(mat);
    }
}

Model::Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
    m_meshes.push_back(std::move(mesh));

    if (!material)
        material = Material::GetError();

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

        auto& shader = mat->GetShader();
        if (shader.IsValid())
        {
            shader.SetMatrix4("model", GetTransformMatrix());
            shader.SetMatrix4("view", g_Engine->GetRenderer()->GetCamera()->GetViewMatrix());
            shader.SetMatrix4("projection", g_Engine->GetRenderer()->GetCamera()->GetProjectionMatrix());
        }

        m_meshes[i]->Draw();
    }
}

void Model::SetMaterial(std::shared_ptr<Material> material, size_t index)
{
    if (!material)
        throw std::invalid_argument("Material is null");
    if (index >= m_materials.size())
        throw std::out_of_range("Invalid material index");

    if (m_materials[index])
        m_materials[index]->Dispose();

    m_materials[index] = material;
}

std::shared_ptr<Material> Model::GetMaterial(size_t index) const
{
    return (index < m_materials.size()) ? m_materials[index] : nullptr;
}

std::shared_ptr<Material> Model::GetMaterial() const
{
    return !m_materials.empty() ? m_materials[0] : nullptr;
}

std::shared_ptr<Mesh> Model::GetMesh(size_t index) const
{
    return (index < m_meshes.size()) ? m_meshes[index] : nullptr;
}

void Model::Dispose()
{
    for (auto& mesh : m_meshes)
        if (mesh) mesh->Dispose();

    for (auto& mat : m_materials)
        if (mat) mat->Dispose();
}