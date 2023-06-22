#pragma once

#include "scope.hpp"
#include <stdexcept>

namespace graphics
{
    template <typename T>
    concept TextureService = requires(T t)
    {
        {t.get_texture_id()} -> std::same_as<unsigned int>;
        {t.get_width()} -> std::same_as<unsigned int>;
        {t.get_height()} -> std::same_as<unsigned int>;
    };

    template <TextureService T>
    class Frame
    {
    private:
        unsigned int fbo_id;
        unsigned int rbo_id;
        T& texture;
    
    public:
        /**
         * @brief Construct a new Frame object
         * @warning throw std::runtime_error when OpenGL framebuffer not complete
         *
         * @param t 
         */
        Frame(T& t) noexcept(false)
            : texture(t)
        {
            Scope([&]()
            {
                glGenFramebuffers(1,&fbo_id);
                glBindFramebuffer(GL_FRAMEBUFFER,fbo_id);

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

        /**
         * @brief Frame can't be copied
         * 
         */
        Frame(Frame&) = delete;

        /**
         * @brief Destroy the Frame object
         * 
         */
        ~Frame() noexcept
        {
            glDeleteRenderbuffers(1,&rbo_id);
            glDeleteFramebuffers(1,&fbo_id);
        }

        /**
         * @brief bind this framebuffer to OpenGL
         * 
         */
        void use() const noexcept
        {
            glBindFramebuffer(GL_FRAMEBUFFER,fbo_id);
        }

        /**
         * @brief bind this framebuffer to OpenGL (output only)
         * 
         */
        void use_as_out() const noexcept
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo_id);
        }

        /**
         * @brief bind this framebuffer to OpenGL (read only)
         * 
         */
        void use_as_read() const noexcept
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo_id);
        }

        /**
         * @brief Get the fbo id object
         * 
         * @return unsigned int 
         */
        unsigned int get_fbo_id() const noexcept
        {
            return fbo_id;
        }

        /**
         * @brief Get the texture id object
         * 
         * @return unsigned int 
         */
        unsigned int get_texture_id() const noexcept
        {
            return texture.get_texture_id();
        }

        /**
         * @brief Get the width object
         * 
         * @return unsigned int 
         */
        unsigned int get_width() const noexcept
        {
            return texture.get_width();
        }

        /**
         * @brief Get the height object
         * 
         * @return unsigned int 
         */
        unsigned int get_height() const noexcept
        {
            return texture.get_height();
        }

        void fill_color(float r,float g,float b,float a) const noexcept
        {
            Scope([&]()
            {
                use();
                glClearColor(r,g,b,a);
            });
        }

        void clear_color_buffer() const noexcept
        {
            Scope([&]()
            {
                use();
                glClear(GL_COLOR_BUFFER_BIT);
            });
        }

        void clear_stencil_buffer() const noexcept
        {
            Scope([&]()
            {
                use();
                glClear(GL_STENCIL_BUFFER_BIT);
            });
        }

        void clear_depth_buffer() const noexcept
        {
            Scope([&]()
            {
                use();
                glClear(GL_DEPTH_BUFFER_BIT);
            });
        }
    };

    class ScreenFrame
    {
    public:
        ScreenFrame() noexcept = delete;

        static void use() noexcept
        {
            glBindFramebuffer(GL_FRAMEBUFFER,0);
        }

        static void use_as_read() noexcept
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
        }

        static void use_as_out() noexcept
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        }

        static unsigned int get_width() noexcept
        {
            int dims[4];
            glGetIntegerv(GL_VIEWPORT,dims);
            return dims[2];
        }

        static unsigned int get_height() noexcept
        {
            int dims[4];
            glGetIntegerv(GL_VIEWPORT,dims);
            return dims[3];
        }

        static void fill_color(float r,float g,float b,float a) noexcept
        {
            Scope([&]()
            {
                use();
                glClearColor(r,g,b,a);
            });
        }

        static void clear_color_buffer() noexcept
        {
            Scope([&]()
            {
                use();
                glClear(GL_COLOR_BUFFER_BIT);
            });
        }

        static void clear_stencil_buffer() noexcept
        {
            Scope([&]()
            {
                use();
                glClear(GL_STENCIL_BUFFER_BIT);
            });
        }

        static void clear_depth_buffer() noexcept
        {
            Scope([&]()
            {
                use();
                glClear(GL_DEPTH_BUFFER_BIT);
            });
        }
    };
}