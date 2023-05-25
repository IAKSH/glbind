#pragma once

#include <glad/glad.h>

namespace rkki::glbind
{
    // can't work yet
    // TODO: learn from zeno
    class Scope
    {
    public:
        Scope()
        {
            //glPushAttrib(GL_ALL_ATTRIB_BITS);
            //glPushClientAttrib(GL_ALL_ATTRIB_BITS);
            //glPushMatrix();
        }

        ~Scope()
        {
            //glPopMatrix();
            //glPopClientAttrib();
            //glPopAttrib();
        }
    };
}