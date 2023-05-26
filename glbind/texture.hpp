#pragma once

#include "concept.hpp"
#include "scope.hpp"
#include <glad/glad.h>

namespace rkki::glbind
{
    template <ColorChannelType img_channel_type,ColorChannelType channel_type>
    class Texture
    {
    private:
        unsigned int texture_id;
        const unsigned int width;
        const unsigned int height;

    public:
        Texture(const unsigned char* const data,
            unsigned int x,unsigned int y,unsigned int w,unsigned int h) noexcept
            : width(w),height(h)
        {
            Scope([&]()
            {
                glGenTextures(1,&texture_id);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

                int img_channel_bits;
                int img_channel_enum;
                if constexpr(img_channel_type == ColorChannelType::Red)
                {
                    img_channel_bits = 1;
                    img_channel_enum = GL_RED;
                }
                else if constexpr(img_channel_type == ColorChannelType::Green)
                {
                    img_channel_bits = 1;
                    img_channel_enum = GL_GREEN;
                }
                else if constexpr(img_channel_type == ColorChannelType::Blue)
                {
                    img_channel_bits = 1;
                    img_channel_enum = GL_BLUE;
                }
                else if constexpr(img_channel_type == ColorChannelType::Aplha)
                {
                    img_channel_bits = 1;
                    img_channel_enum = GL_ALPHA;
                }
                else if constexpr(img_channel_type == ColorChannelType::RGB)
                {
                    img_channel_bits = 3;
                    img_channel_enum = GL_RGB;
                }
                else if constexpr(img_channel_type == ColorChannelType::RGBA)
                {
                    img_channel_bits = 4;
                    img_channel_enum = GL_RGBA;
                }

                int texture_channel_enum;
                if constexpr(channel_type == ColorChannelType::Red)
                    texture_channel_enum = GL_RED;
                else if constexpr(channel_type == ColorChannelType::Green)
                    texture_channel_enum = GL_GREEN;
                else if constexpr(channel_type == ColorChannelType::Blue)
                    texture_channel_enum = GL_BLEND;
                else if constexpr(channel_type == ColorChannelType::Aplha)
                    texture_channel_enum = GL_ALPHA;
                else if constexpr(channel_type == ColorChannelType::RGB)
                    texture_channel_enum = GL_RGB;
                else if constexpr(channel_type == ColorChannelType::RGBA)
                    texture_channel_enum = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D,texture_id);
                glTexImage2D(GL_TEXTURE_2D,0,texture_channel_enum,w,h,0,img_channel_enum,GL_UNSIGNED_BYTE, data + (y * width + x) * img_channel_bits);
                glGenerateMipmap(GL_TEXTURE_2D);
            });
        }

        Texture(Texture&) = delete;

        ~Texture() noexcept
        {
            glDeleteTextures(1,&texture_id);
        }

        void bind() const noexcept
        {
            glBindTexture(GL_TEXTURE_2D,texture_id);
        }

        auto get_texture_id() const noexcept
        {
            return texture_id;
        }

        auto get_width() const noexcept
        {
            return width;
        }

        auto get_height() const noexcept
        {
            return height;
        }

        auto get_channel_type() const noexcept
        {
            return channel_type;
        }
    };

    template <ColorChannelType img_channel_type>
    using TextureRGBA = Texture<ColorChannelType::RGBA,img_channel_type>;
    
    template <ColorChannelType img_channel_type>
    using TextureRGB = Texture<ColorChannelType::RGB,img_channel_type>;

    template <ColorChannelType img_channel_type>
    using TextureR = Texture<ColorChannelType::Red,img_channel_type>;

    template <ColorChannelType img_channel_type>
    using TextureG = Texture<ColorChannelType::Green,img_channel_type>;

    template <ColorChannelType img_channel_type>
    using TextureB = Texture<ColorChannelType::Blue,img_channel_type>;
}