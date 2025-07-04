#pragma once

class IMaterial;

class IMesh 
{
public:
    virtual ~IMesh() = default;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Destroy() = 0;

    virtual void SetMaterial(IMaterial* material) = 0;
    virtual IMaterial* GetMaterial() const = 0;

    virtual void Draw() = 0;
};