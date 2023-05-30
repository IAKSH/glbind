#pragma once

#include "concept.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdexcept>

namespace rkki::glbind
{   
    template <ShaderType type>
    class Shader
    {
    private:
        unsigned int shader_id;

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

        constexpr void compile_vshader(std::string_view glsl) noexcept(false)
        {
            const char* source = glsl.data();
            shader_id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(shader_id,1,&source,nullptr);
            glCompileShader(shader_id);

            check_compile_status();
        }

        constexpr void compile_fshader(std::string_view glsl) noexcept(false)
        {
            const char* source = glsl.data();
            shader_id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(shader_id,1,&source,nullptr);
            glCompileShader(shader_id);

            check_compile_status();
        }

    public:
        Shader(std::string_view glsl) noexcept(false)
        {
            if constexpr(type == ShaderType::VertexShader)
                compile_vshader(glsl);
            else if constexpr(type == ShaderType::FragmentShader)
                compile_fshader(glsl);
        }

        Shader(Shader&) = delete;

        ~Shader() noexcept
        {
            glDeleteShader(shader_id);
        }

        auto get_shader_id() const noexcept
        {
            return shader_id;
        }
    };

    using VShader = Shader<ShaderType::VertexShader>;
    using FShader = Shader<ShaderType::FragmentShader>;

    class Program
    {
    private:
        unsigned int program_id;

        auto get_uniform_location(std::string_view uniform) noexcept(false)
        {
            int location = glGetUniformLocation(program_id,uniform.data());
            if(location == -1)
                throw std::runtime_error("opengl uniform not found");
            
            return location;
        }

    public:
        template <ShaderService VShader,ShaderService FShader>
        Program(const VShader& vshader,const FShader& fshader) noexcept
        {
            program_id = glCreateProgram();
            glAttachShader(program_id,vshader.get_shader_id());
            glAttachShader(program_id,fshader.get_shader_id());
            glLinkProgram(program_id);
        }

        Program(Program&) = delete;

        ~Program() noexcept
        {
            glDeleteProgram(program_id);
        }

        void use() const noexcept
        {
            glUseProgram(program_id);
        }

        auto get_program_id() const noexcept
        {
            return program_id;
        }

        void active_texture_uniform(std::string_view tex_uniform,unsigned int tex_mark)
        {
            glUniform1i(glGetUniformLocation(program_id,tex_uniform.data()),tex_mark);
        }

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