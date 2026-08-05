#pragma once

#include <vector>
#include "imaterial.hpp"
#include "imesh.hpp"
#include "common/transformable.hpp"

//-----------------------------------------------------------------------------
// Model isn't owner of meshes and materials. It just handler which render them.
//-----------------------------------------------------------------------------
class ModelRenderer : public Transformable
{
public:
    ModelRenderer( const std::vector<IMesh*>& meshes, const std::vector<IMaterial*>& materials );
    ModelRenderer( IMesh* mesh, IMaterial* material );

    void SetMaterial( IMaterial* material, size_t index = 0 );

    IMaterial* GetMaterial() const;
    IMaterial* GetMaterial( size_t index ) const;
    std::vector<IMaterial*>& GetMaterials();

    IMesh* GetMesh() const;
    IMesh* GetMesh( size_t index ) const;
    std::vector<IMesh*>& GetMeshes();

    void Draw() const;

    bool m_CastShadows = true;

protected:
    std::vector<IMesh*> m_meshes;
    std::vector<IMaterial*> m_materials;
};
