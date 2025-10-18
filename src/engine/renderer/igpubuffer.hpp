enum class GPUBufferType 
{
    VERTEX,
    INDEX,
    VS_CONSTANT,
    PS_CONSTANT,
};

class IGPUBuffer 
{
public:
    virtual ~IGPUBuffer() = default;
    virtual void Bind(std::uint32_t slot = 0) = 0;
    virtual void Unbind(std::uint32_t slot = 0) = 0;
    virtual void Update(const void* data, size_t size) = 0;
    virtual void Release() = 0;
};