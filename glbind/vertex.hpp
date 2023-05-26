#pragma once

#include "concept.hpp"
#include "scope.hpp"

namespace rkki::glbind
{
    template <VertexBufferType buffer_type,
        size_t vertices_count,size_t indices_count>
    class VertexArray
    {
    private:
        inline static constexpr size_t single_vertex_len {9};
        inline static constexpr size_t single_index_len {3};
        unsigned int vao_id;
        unsigned int vbo_id;
        unsigned int ebo_id;
        using VerticesArray = std::array<float,vertices_count * single_vertex_len>;
        using IndicesArray = std::array<unsigned int,indices_count * single_index_len>;
        VerticesArray vertices;
        IndicesArray indices;
    
    public:
        VertexArray(const VerticesArray& vertices,const IndicesArray& indices) noexcept
            : vertices(vertices),indices(indices)
        {
            Scope([&]()
            {
                int buffer_type_enum;
                if constexpr(buffer_type == VertexBufferType::Static)
                    buffer_type_enum = GL_STATIC_DRAW;
                else if constexpr(buffer_type == VertexBufferType::Dynamic)
                    buffer_type_enum = GL_DYNAMIC_DRAW;
                else if constexpr(buffer_type == VertexBufferType::Stream)
                    buffer_type_enum = GL_STREAM_DRAW;

                glGenVertexArrays(1,&vao_id);
                glGenBuffers(1,&vbo_id);
                glGenBuffers(1,&ebo_id);
                glBindVertexArray(vao_id);
                glBindBuffer(GL_ARRAY_BUFFER,vbo_id);
                glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices.data(),buffer_type_enum);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_id);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices.data(),buffer_type_enum);
                // position attribute
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,9 * sizeof(float),(void*)0);
                glEnableVertexAttribArray(0);
                // color attribute
                glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,9 * sizeof(float),(void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                // texture coord attribute
                glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,9 * sizeof(float),(void*)(7 * sizeof(float)));
                glEnableVertexAttribArray(2);
            });
        }

        VertexArray(VertexArray&) = delete;

        ~VertexArray() noexcept
        {
            glDeleteVertexArrays(1,&vao_id);
            glDeleteBuffers(1,&vbo_id);
        }

        void draw() const noexcept
        {
            Scope([&]()
            {
                glBindVertexArray(vao_id);
                glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            });
        }

        void update() const noexcept
        {
            Scope([&]()
            {
                unsigned int buffer_type_enum;
                if constexpr(buffer_type == VertexBufferType::Static)
                    buffer_type_enum = GL_STATIC_DRAW;
                else if constexpr(buffer_type == VertexBufferType::Dynamic)
                    buffer_type_enum = GL_DYNAMIC_DRAW;
                else if constexpr(buffer_type == VertexBufferType::Stream)
                    buffer_type_enum = GL_STREAM_DRAW;

                glBindBuffer(GL_ARRAY_BUFFER,get_vbo_id());
                glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices.data(),buffer_type_enum);
            });
        }

        auto get_vao_id() const noexcept
        {
            return vao_id;
        }

        auto get_vbo_id() const noexcept
        {
            return vbo_id;
        }

        auto get_ebo_id() const noexcept
        {
            return ebo_id;
        }

        auto get_vertices_len() const noexcept
        {
            return vertices_count * single_vertex_len;
        }

        auto get_indices_len() const noexcept
        {
            return indices_count * single_index_len;
        }

        auto& get_vertices() noexcept
        {
            return vertices;
        }

        auto& get_indices() noexcept
        {
            return indices;
        }
    };
}