#pragma once

#include "vertex.hpp"
#include <exception>

namespace graphics
{
    template <typename T>
    concept VertexArrayService = requires(T t)
    {
        {t.get_vao_id()} -> std::same_as<unsigned int>;
        {t.get_binding_vbo_id()} -> std::same_as<unsigned int>;
        {t.get_binding_ebo_id()} -> std::same_as<unsigned int>;
    };

    enum class Primitives
    {
        POINTS                  = GL_POINTS,
        Lines                   = GL_LINES,
        LineStrip               = GL_LINE_STRIP,
        LineLoop                = GL_LINE_LOOP,
        LinesAdjacency          = GL_LINES_ADJACENCY,
        LinesStripAdjacency     = GL_LINE_STRIP_ADJACENCY,
        Triangles               = GL_TRIANGLES,
        TriangleStrip           = GL_TRIANGLE_STRIP,
        TriangleFan             = GL_TRIANGLE_FAN,
        TriangleAdjacency       = GL_TRIANGLES_ADJACENCY,
        TriangleStripAdjacency  = GL_TRIANGLE_STRIP_ADJACENCY
    };

    /**
     * @brief draw the specified primitive from some points in the vertex array
     * 
     * @tparam primitive 
     * @tparam VAO 
     * @param vao 
     * @param vertex_count 
     */
    template <Primitives primitive,VertexArrayService VAO>
    inline void draw(const VAO& vao,std::size_t vertex_count) noexcept
    {
        Scope([&]()
        {
            glBindVertexArray(vao.get_vao_id());
            glBindBuffer(GL_ARRAY_BUFFER,vao.get_binding_vbo_id());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vao.get_binding_ebo_id());
            glDrawElements(static_cast<int>(primitive),vertex_count,GL_UNSIGNED_INT,0);
        });
    }
}