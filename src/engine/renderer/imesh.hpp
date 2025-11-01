#pragma once

#include "irenderresource.hpp"

class IMaterial;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IMesh : public IRenderResource
{
public:
    virtual ~IMesh() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual void Draw() = 0;
};