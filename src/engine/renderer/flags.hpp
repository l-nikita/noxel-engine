#pragma once

#include "common/utils.hpp"

namespace render
{
    enum class flags : std::uint8_t
    {
        NONE = 0,
        NO_DRAW = 1 << 0,
        NO_SHADOWS = 1 << 1,
    };

    DEFINE_ENUM_FLAG_OPERATORS( flags );
}
