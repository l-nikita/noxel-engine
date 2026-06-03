#pragma once

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IRenderResource
{
public:
    virtual ~IRenderResource() = default;
    virtual void Release() = 0;

    bool IsReleased() const
    {
        return m_isReleased;
    }

protected:
    bool m_isReleased = false;
};