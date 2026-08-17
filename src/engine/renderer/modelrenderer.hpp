#pragma once

#include <cstddef>
#include <vector>

#include "common/transformable.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-------------------------------------------------------------------------

    class Material;
    class Mesh;

    //-------------------------------------------------------------------------
    // ModelRenderer does not own its meshes or materials. Their lifetime must
    // be managed by the renderer's resource/asset manager.
    //-------------------------------------------------------------------------
    struct ModelPart
    {
        Mesh* MeshResource = nullptr;
        Material* MaterialResource = nullptr;
    };

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    class ModelRenderer final : public Transformable<ModelRenderer>
    {
    public:
        ModelRenderer() = default;
        ModelRenderer( Mesh* mesh, Material* material = nullptr );
        ModelRenderer( const std::vector<Mesh*>& meshes, const std::vector<Material*>& materials );

        void AddPart( Mesh* mesh, Material* material = nullptr );

        void SetMesh( Mesh* mesh, std::size_t index = 0 );
        void SetMaterial( Material* material, std::size_t index = 0 );

        Mesh* GetMesh( std::size_t index = 0 ) const;
        Material* GetMaterial( std::size_t index = 0 ) const;

        const ModelPart* GetPart( std::size_t index ) const;
        const std::vector<ModelPart>& GetParts() const;
        std::size_t GetPartCount() const;

        // The active render pass binds its PSO, SRB, and constant buffers.
        // ModelRenderer only binds mesh buffers and submits the indexed draw.
        void DrawPart( IDeviceContext* context, std::size_t index ) const;

    private:
        std::vector<ModelPart> m_parts;
    };
}
