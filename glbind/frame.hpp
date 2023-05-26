#pragma once

#include "concept.hpp"
#include "scope.hpp"
#include "texture.hpp"
#include <stdexcept>

namespace rkki::glbind
{
    template <TextureService T>
    class Frame
    {
    private:
        unsigned int fbo_id;
        unsigned int rbo_id;
        T& texture;

    public:
        Frame(T& t) noexcept(false)
            : texture(t)
        {
            Scope([&]()
            {
                glGenFramebuffers(1,&fbo_id);
                glBindFramebuffer(GL_FRAMEBUFFER,fbo_id);

                /*
                // create texture
                glGenTextures(1,&texture_id);
                glBindTexture(GL_TEXTURE_2D,texture_id);
                glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                */

                // bind texture to fbo
                glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture.get_texture_id(),0);

                // create rbo
                glGenRenderbuffers(1,&rbo_id);
                glBindRenderbuffer(GL_RENDERBUFFER,rbo_id);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,texture.get_width(),texture.get_height());

                // bind rbo to fbo
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo_id);

                // check framebuffer
                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    throw std::runtime_error("OpenGL framebuffer not complete");
            });
        }

        Frame(Frame&) = delete;

        ~Frame() noexcept
        {
            glDeleteRenderbuffers(1,&rbo_id);
            glDeleteFramebuffers(1,&fbo_id);
        }

        void use() const noexcept
        {
            glBindFramebuffer(GL_FRAMEBUFFER,fbo_id);
        }

        void use_as_out() const noexcept
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo_id);
        }

        void use_as_read() const noexcept
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo_id);
        }

        auto get_fbo_id() const noexcept
        {
            return fbo_id;
        }

        auto get_texture_id() const noexcept
        {
            return texture.get_texture_id();
        }

        auto get_width() const noexcept
        {
            return texture.get_width();
        }

        auto get_height() const noexcept
        {
            return texture.get_height();
        }
    };

    using FrameRBG = Frame<TextureRGB<glbind::ColorChannelType::RGB>>;
}