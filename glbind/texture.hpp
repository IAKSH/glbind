#pragma once

#include "scope.hpp"
#include <memory>

namespace graphics
{
    /**
     * @brief enum class to indicate image channel type
     * 
     */
    enum class ImageChannel
    {
        R,G,B,A,RGB,RGBA
    };

    template <ImageChannel texture_channel>
    class Texture
    {
    private:
        unsigned int texture_id;
        const unsigned int width;
        const unsigned int height;

        /**
         * @brief expand or reduce the image channel
         * @warning this function will copy the whole image data, in order to keep the original data
         *
         * @tparam from 
         * @tparam to 
         * @param data 
         * @return std::unique_ptr<unsigned char[]> 
         */
        template <ImageChannel out_channel_type>
        std::unique_ptr<unsigned char[]> trans_image(const unsigned char* data, unsigned int channels) const noexcept
        {
            if(!data)
                return nullptr;

            // calculate the number of bytes needed for the new image channel format
            unsigned int new_channels;
            if constexpr (out_channel_type == ImageChannel::R) {
                new_channels = 1;
            } else if constexpr (out_channel_type == ImageChannel::G) {
                new_channels = 1;
            } else if constexpr (out_channel_type == ImageChannel::B) {
                new_channels = 1;
            } else if constexpr (out_channel_type == ImageChannel::A) {
                new_channels = 1;
            } else if constexpr (out_channel_type == ImageChannel::RGB) {
                new_channels = 3;
            } else if constexpr (out_channel_type == ImageChannel::RGBA) {
                new_channels = 4;
            }
        
            // allocate memory for the new image data
            auto new_data = std::make_unique<unsigned char[]>(width * height * new_channels);
        
            // set up pointers to the old and new image data
            const auto* src = data;
            auto* dst = new_data.get();
        
            // convert the image data pixel by pixel
            for(unsigned int y = 0; y < height; ++y)
            {
                for(unsigned int x = 0; x < width; ++x)
                {
                    // get the color channels from the source image
                    unsigned int r, g, b, a;
                    if(channels == 1)
                    {
                        r = g = b = *src;
                        a = 255;
                        src += sizeof(unsigned char);
                    }
                    else if(channels == 3)
                    {
                        r = *(src + 0);
                        g = *(src + 1);
                        b = *(src + 2);
                        a = 255;
                        src += sizeof(unsigned char) * 3;
                    }
                    else if(channels == 4)
                    {
                        r = *(src + 0);
                        g = *(src + 1);
                        b = *(src + 2);
                        a = *(src + 3);
                        src += sizeof(unsigned char) * 4;
                    }
        
                    // write the color channels to the destination image
                    if constexpr (out_channel_type == ImageChannel::R)
                    {
                        *dst = r;
                        dst += sizeof(unsigned char);
                    }
                    else if constexpr (out_channel_type == ImageChannel::G)
                    {
                        *dst = g;
                        dst += sizeof(unsigned char);
                    }
                    else if constexpr (out_channel_type == ImageChannel::B)
                    {
                        *dst = b;
                        dst += sizeof(unsigned char);
                    }
                    else if constexpr (out_channel_type == ImageChannel::A)
                    {
                        *dst = a;
                        dst += sizeof(unsigned char);
                    }
                    else if constexpr (out_channel_type == ImageChannel::RGB)
                    {
                        *dst++ = r;
                        *dst++ = g;
                        *dst++ = b;
                    }
                    else if constexpr (out_channel_type == ImageChannel::RGBA)
                    {
                        *dst++ = r;
                        *dst++ = g;
                        *dst++ = b;
                        *dst++ = a;
                    }
                }
            }

            return new_data;
        }

    public:
        /**
         * @brief       Construct a new Texture object
         * 
         * @param data  bitmap data pointer
         * @param x     X axis coordinate of position where to start samping iamge
         * @param y     Y axis coordinate of position where to start samping iamge
         * @param w     width of the texture
         * @param h     height of the texture
         */
        Texture(const unsigned char* const data,unsigned int channels,unsigned int x,unsigned int y,unsigned int w,unsigned int h) noexcept
            : width(w),height(h)
        {
            Scope([&]()
            {
                glGenTextures(1,&texture_id);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

                int texture_channel_enum;
                unsigned int texture_source_channels;
                if constexpr(texture_channel == ImageChannel::R)
                {
                    texture_channel_enum = GL_RED;
                    texture_source_channels = 1;
                }
                else if constexpr(texture_channel == ImageChannel::G)
                {
                    texture_channel_enum = GL_GREEN;
                    texture_source_channels = 1;
                }
                else if constexpr(texture_channel == ImageChannel::B)
                {
                    texture_channel_enum = GL_BLEND;
                    texture_source_channels = 1;
                }
                else if constexpr(texture_channel == ImageChannel::A)
                {
                    texture_channel_enum = GL_ALPHA;
                    texture_source_channels = 1;
                }
                else if constexpr(texture_channel == ImageChannel::RGB)
                {
                    texture_channel_enum = GL_RGB;
                    texture_source_channels = 3;
                }
                else if constexpr(texture_channel == ImageChannel::RGBA)
                {
                    texture_channel_enum = GL_RGBA;
                    texture_source_channels = 4;
                }

                int img_channel_enum;
                if(channels == 1)
                    img_channel_enum = GL_RED;
                else if(channels == 3)
                    img_channel_enum = GL_RGB;
                else if(channels == 4)
                    img_channel_enum = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D,texture_id);
                glTexImage2D(GL_TEXTURE_2D,0,texture_channel_enum,w,h,0,img_channel_enum,GL_UNSIGNED_BYTE, trans_image<texture_channel>(data,channels).get() + (y * width + x) * texture_source_channels);
                glGenerateMipmap(GL_TEXTURE_2D);
            });
        }

        /**
         * @brief Texture can't be copied
         * 
         */
        Texture(Texture&) = delete;

        /**
         * @brief Destroy the Texture object
         * 
         */
        ~Texture() noexcept
        {
            glDeleteTextures(1,&texture_id);
        }

        /**
         * @brief bind this textrue to OpenGL
         * @warning this will change the status of OpenGL
         */
        void bind() const noexcept
        {
            glBindTexture(GL_TEXTURE_2D,texture_id);
        }

        /**
         * @brief Get the texture id
         * 
         * @return unsigned int 
         */
        unsigned int get_texture_id() const noexcept
        {
            return texture_id;
        }

        /**
         * @brief Get the width of this texture
         * 
         * @return unsigned int 
         */
        unsigned int get_width() const noexcept
        {
            return width;
        }

        /**
         * @brief Get the height of this texture
         * 
         * @return unsigned int 
         */
        unsigned int get_height() const noexcept
        {
            return height;
        }

        /**
         * @brief Get the channel type of this texture
         * 
         * @return ImageChannel 
         */
        constexpr ImageChannel get_channel_type() const noexcept
        {
            return texture_channel;
        }
    };

    using TextureR = Texture<ImageChannel::R>;
    using TextureG = Texture<ImageChannel::G>;
    using TextureB = Texture<ImageChannel::B>;
    using TextureA = Texture<ImageChannel::A>;
    using TextureRGB = Texture<ImageChannel::RGB>;
    using TextureRGBA = Texture<ImageChannel::RGBA>;
}