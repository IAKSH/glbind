#pragma once

#include "scope.hpp"
#include <glad/glad.h>
#include <array>
#include <concepts>
#include <stdexcept>
#include <cstddef>
#include <type_traits>

namespace graphics
{
    enum class BufferType
    {
        Static,Dynamic,Stream
    };

    template <BufferType type,std::size_t len>
    class VertexBuffer
    {
    private:
        unsigned int vbo_id;

        /**
         * @brief Create a vbo object and fill Vertex Buffer in OpenGL
         * 
         * @param arr 
         */
        void create_vbo(const std::array<float,len>& arr) noexcept
        {
            glGenBuffers(1,&vbo_id);
            update(arr);
        }

    public:
        /**
         * @brief Construct a new Vertex Buffer object (with all vertex data = 0.0f)
         * 
         */
        VertexBuffer() noexcept
        {
            create_vbo({});
        }

        /**
         * @brief Construct a new Vertex Buffer object
         * 
         * @param arr 
         */
        VertexBuffer(const std::array<float,len>& arr) noexcept
        {
            create_vbo(arr);
        }

        /**
         * @brief VertexBuffer can't be copied
         * 
         */
        VertexBuffer(VertexBuffer&) = delete;

        /**
         * @brief Destroy the Vertex Buffer object
         * 
         */
        ~VertexBuffer() noexcept
        {
            glDeleteBuffers(1,&vbo_id);
        }

        /**
         * @brief update Vertex Buffer in OpenGL
         * 
         * @param arr 
         */
        void update(const std::array<float,len>& arr) const noexcept
        {
            Scope([&]()
            {
                int buffer_type_enum;
                if constexpr(type == BufferType::Static)
                    buffer_type_enum = GL_STATIC_DRAW;
                else if constexpr(type == BufferType::Dynamic)
                    buffer_type_enum = GL_DYNAMIC_DRAW;
                else if constexpr(type == BufferType::Stream)
                    buffer_type_enum = GL_STREAM_DRAW;
                
                glBindBuffer(GL_ARRAY_BUFFER,vbo_id);
                glBufferData(GL_ARRAY_BUFFER,sizeof(arr),arr.data(),buffer_type_enum);
            });
        }

        /**
         * @brief Get the vbo id object
         * 
         * @return unsigned int 
         */
        unsigned int get_vbo_id() const noexcept
        {
            return vbo_id;
        }

        /**
         * @brief Get the len object
         * 
         * @return constexpr std::size_t 
         */
        constexpr std::size_t get_len() const noexcept
        {
            return len;
        }
    };

    template <BufferType type,std::size_t len>
    class ElementBuffer
    {
    private:
        unsigned int ebo_id;

    public:
        /**
         * @brief Construct a new Element Buffer object
         * 
         * @param arr 
         */
        ElementBuffer(const std::array<unsigned int,len>& arr) noexcept
        {
            glGenBuffers(1,&ebo_id);
            update(arr);
        }

        /**
         * @brief ElementBuffer can't be copied
         * 
         */
        ElementBuffer(ElementBuffer&) = delete;

        /**
         * @brief Destroy the Element Buffer object
         * 
         */
        ~ElementBuffer() noexcept
        {
            glDeleteBuffers(1,&ebo_id);
        }

        /**
         * @brief update Element Buffer in OpenGL
         * 
         * @param arr 
         */
        void update(const std::array<unsigned int,len>& arr) const noexcept
        {
            Scope([&]()
            {
                unsigned int buffer_type_enum;
                if constexpr(type == BufferType::Static)
                    buffer_type_enum = GL_STATIC_DRAW;
                else if constexpr(type == BufferType::Dynamic)
                    buffer_type_enum = GL_DYNAMIC_DRAW;
                else if constexpr(type == BufferType::Stream)
                    buffer_type_enum = GL_STREAM_DRAW;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_id);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(arr),arr.data(),buffer_type_enum);
            });
        }

        /**
         * @brief Get the ebo id object
         * 
         * @return unsigned int 
         */
        unsigned int get_ebo_id() const noexcept
        {
            return ebo_id;
        }

        /**
         * @brief Get the len object
         * 
         * @return constexpr std::size_t 
         */
        constexpr std::size_t get_len() const noexcept
        {
            return len;
        }
    };

    template <typename T>
    concept VertexBufferService = requires(T t)
    {
        {t.get_vbo_id()} -> std::same_as<unsigned int>;
    };

    template <typename T>
    concept ElementBufferService = requires(T t)
    {
        {t.get_ebo_id()} -> std::same_as<unsigned int>;
    };

    template <VertexBufferService VBO>
    class VertexArray
    {
    private:
        const VBO& vbo;
        unsigned int vao_id;

    public:
        VertexArray(const VBO& vbo) noexcept
            : vbo(vbo)
        {
            glGenVertexArrays(1,&vao_id);
        }

        VertexArray(VertexArray&) noexcept = delete;

        ~VertexArray() noexcept
        {
            glDeleteVertexArrays(1,&vao_id);
        }

        unsigned int get_vao_id() const noexcept
        {
            return vao_id;
        }

        unsigned int get_binding_vbo_id() const noexcept
        {
            return vbo.get_vbo_id();
        }

        /**
         * @brief               bind vertex attrib pointer for OpenGL
         * 
         * @param index         the index of vertex data (used in OpenGL GLSL) 
         * @param len           the lenth of this attrib (by count)
         * @param vertex_len    the lenth of a single vertex data (by count)
         * @param offset        offset of this attrib in the whole single vertex data
         * @param normalized    if need to normalize vertices data (int to float between [-1,1] or [0,1])
         */
        void enable_attrib(unsigned int index,std::size_t len,std::size_t vertex_len,std::size_t offset,bool normalized = false) const noexcept
        {
            Scope([&]()
            {
                glBindBuffer(GL_ARRAY_BUFFER,vbo.get_vbo_id());
                //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo.get_ebo_id());
                glBindVertexArray(vao_id);
                
                glVertexAttribPointer(index,len,GL_FLOAT,normalized,vertex_len * sizeof(float),(void*)(offset * sizeof(float)));
                glEnableVertexAttribArray(index);
            });
        }
    };

    template <VertexBufferService VBO,ElementBufferService EBO>
    class VertexArrayWithEBO : public VertexArray<VBO>
    {
    private:
        const EBO& ebo;

    public:
        /**
         * @brief Construct a new Vertex Array object (with ebo)
         * 
         * @param vbo 
         * @param ebo 
         */
        VertexArrayWithEBO(const VBO& vbo,const EBO& ebo) noexcept
            : VertexArray<VBO>(vbo),ebo(ebo)
        {
        }

        /**
         * @brief Destroy the Vertex Array object
         * 
         */
        ~VertexArrayWithEBO() noexcept = default;

        unsigned int get_binding_ebo_id() const noexcept
        {
            return ebo.get_ebo_id();
        }
    };
}