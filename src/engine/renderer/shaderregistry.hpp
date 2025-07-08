#pragma once

#include <functional>
#include <unordered_map>
#include <memory>
#include "ishader.hpp"
#include "ishaderprogram.hpp"

class ShaderRegistry
{
public:
    using ShaderCreator = std::function<IShader* (IShaderProgram*)>;

    static std::unordered_map<std::string, ShaderCreator>& Registry()
    {
        static std::unordered_map<std::string, ShaderCreator> registry;
        return registry;
    }

    static void Register(const std::string& type, ShaderCreator creator)
    {
        Registry()[type] = creator;
    }

    static IShader* Create(const std::string& type, IShaderProgram* shaderProg)
    {
        auto it = Registry().find(type);
        if (it != Registry().end())
            return it->second(shaderProg);

        return nullptr;
    }
};

#define REGISTER_SHADER(TYPE_NAME, CLASS_NAME) \
    namespace { \
        struct CLASS_NAME##Registrator { \
            CLASS_NAME##Registrator() { \
                ShaderRegistry::Register(TYPE_NAME, [](IShaderProgram* program) { \
                    return new CLASS_NAME(program); \
                }); \
            } \
        }; \
        static CLASS_NAME##Registrator global_##CLASS_NAME##_registrator; \
    }