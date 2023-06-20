#pragma once

#include <array>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>

namespace graphics::extension
{
    class Camera
    {
    private:
        glm::quat orientation;
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 position;
        float fov;
        unsigned int view_width;
        unsigned int view_height;

    public:
        Camera() noexcept;
        Camera(unsigned int w,unsigned int h) noexcept;
        ~Camera() noexcept = default;
        float get_fov() const noexcept;
        std::array<float,3> get_position() const noexcept;
        glm::mat4 get_matrix() const noexcept;
        void set_fov(float fov) noexcept;
        void set_position(std::array<float,3> arr) noexcept;
        void rotate(float d_up,float d_right,float d_roll) noexcept;
        void move(float d_front,float d_right,float d_height) noexcept;
    };
}