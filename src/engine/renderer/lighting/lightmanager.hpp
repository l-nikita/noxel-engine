#pragma once

#include "renderer/rendersystem.hpp"
#include "renderer/lighting/light.hpp"
#include "renderer/lighting/lightcommon.hpp"
#include "renderer/lighting/shadowmap.hpp"

namespace render::lighting
{
    //TODO: make shadow pool

    //-----------------------------------------------------------------------------

    struct LightData
    {
        std::unique_ptr<Light> Light;
        std::unique_ptr<ShadowMap> ShadowMap;
    };

    //-----------------------------------------------------------------------------

    using Lights = std::vector<LightData>;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Manager final
    {
    public:
        explicit Manager( RenderSystem* render )
            : m_render( render )
        {
            m_lights.reserve( MAX_LIGHTS );
        }

        ~Manager()
        {
            m_lights.clear();
        }

        template<typename... Args>
        const LightData& CreateLight( Args&&... args )
        {
            std::unique_ptr<Light> light = std::make_unique<Light>( std::forward<Args>( args )... );
            std::unique_ptr<ShadowMap> shadowMap =
                    std::make_unique<ShadowMap>( m_render->GetDevice(), SHADOW_MAP_SIZE, light->IsShadowsEnabled() );

            m_lights.push_back( {std::move( light ), std::move( shadowMap )} );
            return m_lights.back();
        }

        void RemoveLight( Light* light )
        {
            if ( !light )
                return;

            std::erase_if( m_lights, [light]( const LightData& data )
            {
                return data.Light.get() == light;
            } );
        }

        const Lights& GetLights() const
        {
            return m_lights;
        }

    private:
        Lights m_lights = {};

        RenderSystem* m_render = nullptr;
    };
}
