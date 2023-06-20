#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include "glm/geometric.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

graphics::extension::Camera::Camera() noexcept
    : fov(45.0f),view_width(800),view_height(600),
    orientation(glm::vec3(0.0f,0.0f,-1.0f)),right(1.0f,0.0f,0.0f),up(0.0f,1.0f,0.0f),position(0.0f,0.0f,0.0f)
{
}

graphics::extension::Camera::Camera(unsigned int w,unsigned int h) noexcept
    : fov(45.0f),view_width(w),view_height(h),
    orientation(glm::vec3(0.0f,0.0f,-1.0f)),right(1.0f,0.0f,0.0f),up(0.0f,1.0f,0.0f),position(0.0f,0.0f,0.0f)
{
}

float graphics::extension::Camera::get_fov() const noexcept
{
    return fov;
}

void graphics::extension::Camera::set_fov(float fov) noexcept
{
    this->fov = fov;
}

std::array<float,3> graphics::extension::Camera::get_position() const noexcept
{
    return std::array<float,3>{position[0],position[1],position[2]};
}

glm::mat4 graphics::extension::Camera::get_matrix() const noexcept
{
    glm::vec3 target = position + glm::rotate(orientation,glm::vec3(0.0f, 0.0f, -1.0f));
    return glm::perspective(glm::radians(fov),static_cast<float>(view_width) / view_height,0.1f,20000.0f) * glm::lookAt(position,target,up);
}

void graphics::extension::Camera::set_position(std::array<float,3> arr) noexcept
{
    for(std::size_t i = 0;i < 3;i++)
        position[i] = arr[i];
}

void graphics::extension::Camera::rotate(float d_up, float d_right, float d_roll) noexcept
{
    glm::quat rot_quat = glm::angleAxis(glm::radians(d_right), right) * 
                         glm::angleAxis(glm::radians(d_up), up) *
                         glm::angleAxis(glm::radians(d_roll), orientation * glm::vec3(0, 0, -1));

    orientation = glm::normalize(rot_quat * orientation);
    right = glm::normalize(glm::cross(orientation * glm::vec3(0, 1, 0), orientation * glm::vec3(0, 0, -1)));
    up = glm::normalize(glm::cross(right, orientation * glm::vec3(0, 0, -1)));
}

void graphics::extension::Camera::move(float d_front,float d_right,float d_height) noexcept
{
    position += d_front * glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f)) + d_right * right + d_height * up;
}