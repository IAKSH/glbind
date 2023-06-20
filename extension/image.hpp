#pragma once

#include <fstream>
#include <memory>
#include <string>

namespace graphics::extension
{
    class Image
    {
    private:
        std::unique_ptr<unsigned char[]> data;
        unsigned int width;
        unsigned int height;
        unsigned int channels;

    public:
        Image(std::unique_ptr<unsigned char[]>,unsigned int width,unsigned int height,unsigned int channels) noexcept(false);
        Image(Image& image) noexcept = delete;
        ~Image() noexcept = default;
        const unsigned char* get_data() const noexcept;
        unsigned int get_width() const noexcept;
        unsigned int get_height() const noexcept;
        unsigned int get_channels() const noexcept;
    };
}