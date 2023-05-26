#pragma once

#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <concepts>
#include <stdexcept>
#include <string>

namespace rkki::glbind
{
    template <typename T,typename U>
    constexpr bool is_same()
    {
        using Type = std::remove_cvref_t<T>;
        return std::is_same<Type,U>();
    }

    template <typename T,typename... Args>
    constexpr bool any_same()
    {
        if constexpr(sizeof...(Args) == 0)
            return false;
        else
            return (std::same_as<std::remove_reference_t<T>, Args> || ...) || any_same<Args...>();
    }

    enum class ShaderType
    {
        VertexShader,FragmentShader
    };

    template <typename T>
    concept ShaderService = requires(T t)
    {
        {t.get_shader_id()} -> std::same_as<unsigned int>;
    };

    enum class ColorChannelType
    {
        RGBA,RGB,Red,Green,Blue,Aplha
    };

    template <typename T>
    concept TextureService = requires(T t)
    {
        {t.bind()} -> std::same_as<void>;
        {t.get_texture_id()} -> std::same_as<unsigned int>;
        {t.get_width()} -> std::same_as<unsigned int>;
        {t.get_height()} -> std::same_as<unsigned int>;
        {t.get_channel_type()} -> std::same_as<ColorChannelType>;
    };

    template <typename T>
    concept Uniform = any_same<T,glm::vec2,glm::vec3,glm::vec4,glm::mat4,int,float>() || TextureService<T>;

    template <typename T>
    concept ProgramService = requires(T t)
    {
        {t.use()} -> std::same_as<void>;
        {t.get_program_id()} -> std::same_as<unsigned int>;
        
    };

    template <typename T,typename U>
    concept UniformManagerService = Uniform<U> && requires(T t,const U& uniform,std::string_view uniform_name,unsigned int tex_mark)
    {
        {t.get_uniform()} -> std::same_as<U&&>;
        {t.set_uniform(uniform_name,uniform)} -> std::same_as<void>; 
        {t.active_texture_uniform(uniform_name,tex_mark)} -> std::same_as<void>;
    };

    enum class VertexBufferType
    {
        Static,Dynamic,Stream
    };

    template <typename T,std::size_t vertices_len,std::size_t indices_len>
    concept VertexService = requires(T t)
    {
        {t.draw()} -> std::same_as<void>;
        {t.update()} -> std::same_as<void>;
        {t.get_vao_id()} -> std::same_as<unsigned int>;
        {t.get_vbo_id()} -> std::same_as<unsigned int>;
        {t.get_ebo_id()} -> std::same_as<unsigned int>;
        {t.get_vertices_len()} -> std::same_as<size_t>;
        {t.get_indices_len()} -> std::same_as<size_t>;
        {t.get_vertices()} -> std::same_as<std::array<float,vertices_len>&>;
        {t.get_indices()} -> std::same_as<std::array<float,indices_len>&>;
    };

    template <typename T>
    concept FrameService = requires(T t)
    {
        {t.use()} -> std::same_as<void>;
        {t.use_as_out()} -> std::same_as<void>;
        {t.use_as_read()} -> std::same_as<void>;
        {t.get_fbo_id()} -> std::same_as<unsigned int>;
        {t.get_texture_id()} -> std::same_as<unsigned int>;
    };
}