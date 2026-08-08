#pragma once

#include <array>
#include <expected>
#include <span>
#include <string>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
enum class SysId
{
    FILESYSTEM,
    ASSET_SYSTEM,
    INPUT,
    PHYSICS,
    RENDER,
    RENDER_PIPELINE,
    GUI
};

struct SysMetadata
{
    SysId Id;
    std::string_view Name;
    std::span<const SysId> Dependencies;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
using SysInit = std::expected<void, std::string>;

class ISystem
{
public:
    virtual ~ISystem() = default;

    virtual SysMetadata GetMetadata() const noexcept = 0;
    virtual SysInit Initialize() = 0;
    virtual void Shutdown() = 0;
};

template<SysId... Ids>
inline const std::array<SysId, sizeof...( Ids )> SystemDependencies{
    Ids...
};

#define SYSTEM_METADATA(id, name, ...)                              \
    SysMetadata GetMetadata() const noexcept override               \
    {                                                               \
        static constexpr SysMetadata metadata{                      \
            SysId::id,                                           \
            name,                                                   \
            SystemDependencies<__VA_ARGS__>                         \
        };                                                          \
        return metadata;                                            \
    }
