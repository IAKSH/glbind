#pragma once

#include "concept.hpp"
#include <glad/glad.h>
#include <functional>

namespace rkki::glbind
{
    class Scope
    {
    public:
        explicit Scope(std::function<void()> func)
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushClientAttrib(GL_ALL_ATTRIB_BITS);
            glPushMatrix();
            
            func();
        }

        ~Scope()
        {
            glPopMatrix();
            glPopClientAttrib();
            glPopAttrib();

            glBindFramebuffer(GL_FRAMEBUFFER,0);
            glBindTexture(GL_TEXTURE_2D,0);
            glBindRenderbuffer(GL_RENDERBUFFER,0);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        }
    };
}