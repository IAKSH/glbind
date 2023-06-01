#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <string>
#include <memory>
#include <stdexcept>

namespace graphics
{
    enum class ShaderType
    {
        VertexShader,FragmentShader
    };

    template <ShaderType type>
    class Shader
    {
    private:
        unsigned int shader_id;

        /**
         * @brief       Check if shader compile failed.
         * @warning     throw std::runtime_error when compile failed
         */
        constexpr void check_compile_status() noexcept(false)
        {
            int success;
            char info_log[512];
            glGetShaderiv(shader_id,GL_COMPILE_STATUS,&success);
            if(!success)
            {
                glGetShaderInfoLog(shader_id,sizeof(info_log),nullptr,info_log);
                throw std::runtime_error(info_log);
            }
        }

        /**
         * @brief       Compile glsl to vertex shader.
         * @warning     throw std::runtime_error from check_compile_status() when comiple failed
         *
         * @param glsl  OpenGL GLSL code for vertex shader
         */
        constexpr void compile_vshader(std::string_view glsl) noexcept(false)
        {
            const char* source = glsl.data();
            shader_id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(shader_id,1,&source,nullptr);
            glCompileShader(shader_id);

            check_compile_status();
        }

        /**
         * @brief       Compile glsl to fragment shader.
         * @warning     throw std::runtime_error from check_compile_status() when compile failed
         * 
         * @param glsl  OpenGL GLSL code for fragment shader
         */
        constexpr void compile_fshader(std::string_view glsl) noexcept(false)
        {
            const char* source = glsl.data();
            shader_id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(shader_id,1,&source,nullptr);
            glCompileShader(shader_id);

            check_compile_status();
        }

    public:
        /**
         * @brief           Construct a new Shader object
         * @warning         throw std::runtime_error when compile failed
         *
         * @param glsl_code OpenGL GLSL code
         */
        Shader(std::string_view glsl_code) noexcept(false)
        {
            if constexpr(type == ShaderType::VertexShader)
                compile_vshader(glsl_code);
            else if constexpr(type == ShaderType::FragmentShader)
                compile_fshader(glsl_code);
        }

        /**
         * @brief Shader can not be copied
         * 
         */
        Shader(Shader&) = delete;

        /**
         * @brief Destroy the Shader object
         * 
         */
        ~Shader() noexcept
        {
            glDeleteShader(shader_id);
        }

        /**
         * @brief Get the shader id object
         * 
         * @return unsigned int 
         */
        unsigned int get_shader_id() const noexcept
        {
            return shader_id;
        }
    };

    /**
     * @brief declare VShader and FShader from template class Shader
     * 
     */
    using VShader = Shader<ShaderType::VertexShader>;
    using FShader = Shader<ShaderType::FragmentShader>;

    /**
     * @brief type similarity check in compile time (ignore const or ref on type U)
     * 
     * @tparam T
     * @tparam U
     * @return true
     * @return false 
     */
    template <typename T,typename U>
    constexpr bool is_same()
    {
        using Type = std::remove_cvref_t<T>;
        return std::is_same<Type,U>();
    }
    
    /**
     * @brief muti-arg type similarity check in compile time (ignore const or ref on type U)
     * 
     * @tparam T 
     * @tparam Args 
     * @return true 
     * @return false 
     */
    template <typename T,typename... Args>
    constexpr bool any_same()
    {
        if constexpr(sizeof...(Args) == 0)
            return false;
        else
            return (std::same_as<std::remove_reference_t<T>, Args> || ...) || any_same<Args...>();
    }

    template <typename T>
    concept WithShaderIdAPI = requires(T t)
    {
        {t.get_shader_id()} -> std::same_as<unsigned int>;
    };

    /**
     * @brief supported OpenGL uniform type
     * 
     * @tparam T 
     */
    template <typename T>
    concept Uniform = any_same<T,int,bool,float,glm::vec2,glm::vec3,glm::vec4,glm::mat4>();

    class Program
    {
    private:
        unsigned int program_id;

        /**
         * @brief get OpenGL uniform location by name
         * 
         * @param uniform 
         * @return int
         */
        int get_uniform_location(std::string_view uniform) noexcept(false)
        {
            int location = glGetUniformLocation(program_id,uniform.data());
            if(location == -1)
                throw std::runtime_error("opengl uniform not found");
            
            return location;
        }

    public:
        /**
         * @brief Construct a new Program object
         * 
         * @tparam VShader 
         * @tparam FShader 
         * @param vshader 
         * @param fshader 
         */
        template <WithShaderIdAPI VShader,WithShaderIdAPI FShader>
        Program(const VShader& vshader,const FShader& fshader) noexcept
        {
            program_id = glCreateProgram();
            glAttachShader(program_id,vshader.get_shader_id());
            glAttachShader(program_id,fshader.get_shader_id());
            glLinkProgram(program_id);
        }

        /**
         * @brief Program can't be copied
         * 
         */
        Program(Program&) = delete;

        /**
         * @brief Destroy the Program object
         * 
         */
        ~Program() noexcept
        {
            glDeleteProgram(program_id);
        }

        /**
         * @brief bind this program to OpenGL
         * @warning this will change the status of OpenGL
         */
        void use() const noexcept
        {
            glUseProgram(program_id);
        }

        /**
         * @brief get the program id object
         * 
         * @return unsigned int  
         */
        unsigned int get_program_id() const noexcept
        {
            return program_id;
        }

        /**
         * @brief bind GLSL sample uniform with mark 
         * 
         * @param tex_uniform 
         * @param tex_mark 
         */
        void bind_texture_uniform(std::string_view tex_uniform,unsigned int tex_mark)
        {
            glUniform1i(glGetUniformLocation(program_id,tex_uniform.data()),tex_mark);
        }

        /**
         * @brief set OpenGL uniform
         * 
         * @tparam T 
         * @param uniform 
         * @param t 
         */
        template <Uniform T>
        void set_uniform(std::string_view uniform,const T& t) noexcept(false)
        {
            auto&& location {get_uniform_location(uniform)};
            
            if constexpr(is_same<T,int>() || is_same<T,bool>())
                glUniform1i(location,t);
            else if constexpr(is_same<T,float>())
                glUniform1f(location,t);
            else if constexpr(is_same<T,glm::vec2>())
                glUniform2fv(location,glm::value_ptr(t));
            else if constexpr(is_same<T,glm::vec3>())
                glUniform3fv(location,glm::value_ptr(t));
            else if constexpr(is_same<T,glm::vec4>())
                glUniform4fv(location,glm::value_ptr(t));
            else if constexpr(is_same<T,glm::mat4>())
                glUniformMatrix4fv(location,1,false,glm::value_ptr(t));
            else
                throw std::runtime_error("compile-time uniform type check failed");
        }

        /**
         * @brief get OpenGL uniform
         * 
         * @tparam T 
         * @param uniform 
         * @return T&& 
         */
        template <Uniform T>
        T&& get_uniform(std::string_view uniform) noexcept(false)
        {
            int size;
            unsigned int type;
            auto&& location {get_uniform_location(uniform)};
            glGetActiveUniform(program_id,location,0,nullptr,&size,&type,nullptr);

            unsigned int aimed_type;
            if constexpr(is_same<T,int>())
                aimed_type = GL_INT;
            else if constexpr(is_same<T,float>())
                aimed_type = GL_FLOAT;
            else if constexpr(is_same<T,glm::vec2>())
                aimed_type = GL_FLOAT_VEC2;
            else if constexpr(is_same<T,glm::vec3>())
                aimed_type = GL_FLOAT_VEC3;
            else if constexpr(is_same<T,glm::vec4>())
                aimed_type = GL_FLOAT_VEC4;
            else if constexpr(is_same<T,glm::mat4>())
                aimed_type = GL_FLOAT_MAT4;
            else
                throw std::runtime_error("compile-time uniform type check failed");

            if(size != 1)
                throw std::runtime_error("uniform name conflict");
            if(type != aimed_type)
                throw("unsupported uniform type");
            
            T ret_uniform;
            if constexpr(is_same<T,int>())
                glGetUniformiv(program_id,location,&ret_uniform);
            else if constexpr(is_same<T,float>())
                glGetUniformfv(program_id,location,&ret_uniform);
            else
                glGetUniformfv(program_id,location,glm::value_ptr(ret_uniform));
            
            return ret_uniform;
        }
    };
}