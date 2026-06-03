#pragma once

class Panel
{
public:
    Panel();
    virtual ~Panel() = default;

    bool m_IsVisible;
    virtual void Draw() = 0;
};