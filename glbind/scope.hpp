#pragma once

#include <glad/glad.h>
#include <functional>

namespace graphics
{
    /**
     * @brief Clear up some opengl status after calling func.
     * 
     * @param func the (lambda) func you wish to call
     */
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

    /**
     * @brief Reset opengl view firstly then call func and clear up status
     * 
     * @param x     opengl view X
     * @param y     opengl view Y
     * @param w     opengl view width
     * @param h     opengl view height
     * @param func  the (lambda) func you wish to call
     */
    inline void Scope(int x,int y,int w,int h,std::function<void()> func)
    {
        glViewport(x,y,w,h);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Scope(func);
    }

    /**
     * @brief Reset view and clear background with color then call func and clear up status
     * 
     * @param x     opengl view X
     * @param y     opengl view Y
     * @param w     opengl view width
     * @param h     opengl view height
     * @param r     background color (red)
     * @param g     background color (green)
     * @param b     background color (blue)
     * @param a     background color (alpha)
     * @param func  the (lambda) func you wish to call
     */
    inline void Scope(int x,int y,int w,int h,float r,float g,float b,float a,std::function<void()> func)
    {
        glViewport(x,y,w,h);
        glClearColor(r,g,b,a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        Scope(func);
    }
}