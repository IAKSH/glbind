#include "image.hpp"
#include <stdexcept>

graphics::extension::Image::Image(std::unique_ptr<unsigned char[]> data,unsigned int width,unsigned int height,unsigned int channels)
    : data(std::move(data)),width(width),height(height),channels(channels)
{
}

const unsigned char* graphics::extension::Image::get_data() const noexcept
{
    return data.get();
}

unsigned int graphics::extension::Image::get_width() const noexcept
{
    return width;
}

unsigned int graphics::extension::Image::get_height() const noexcept
{
    return height;
}

unsigned int graphics::extension::Image::get_channels() const noexcept
{
    return channels;
}