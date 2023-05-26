#pragma once

#include "concept.hpp"
#include <glad/glad.h>
#include <functional>

namespace rkki::glbind
{
    inline void Scope(std::function<void()> func)
    {
        int framebuffer_id;
        int texture_2d_id;
        int renderbuffer_id;
        int vao_id;
        int vbo_id;
        int ebo_id;

        // storage status
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_ALL_ATTRIB_BITS);
        glPushMatrix();
        glGetIntegerv(GL_FRAMEBUFFER_BINDING,&framebuffer_id);
        glGetIntegerv(GL_TEXTURE_BINDING_2D,&texture_2d_id);
        glGetIntegerv(GL_RENDERBUFFER_BINDING,&renderbuffer_id);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&vao_id);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&vbo_id);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&ebo_id);

        func();

        // recovery
        glPopMatrix();
        glPopClientAttrib();
        glPopAttrib();
        glBindFramebuffer(GL_FRAMEBUFFER,framebuffer_id);
        glBindTexture(GL_TEXTURE_2D,texture_2d_id);
        glBindRenderbuffer(GL_RENDERBUFFER,renderbuffer_id);
        glBindVertexArray(vao_id);
        glBindBuffer(GL_ARRAY_BUFFER,vbo_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_id);
    }
}