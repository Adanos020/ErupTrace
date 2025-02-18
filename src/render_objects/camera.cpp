#include <render_objects/camera.hpp>

#include <util/geometric.hpp>
#include <util/random.hpp>

camera::camera(const camera_create_info& info)
    : origin(info.camera_position)
    , w(glm::normalize(info.camera_position - info.looking_at))
    , u(glm::normalize(glm::cross(info.up, this->w)))
    , v(glm::cross(this->w, this->u))
    , lens_radius(info.aperture * 0.5f)
    , time(info.time)
{
    const float half_height = glm::tan(glm::radians(info.vertical_fov) * 0.5f);
    const float half_width = info.aspect_ratio * half_height;
    const float focus_distance = glm::distance(info.camera_position, info.looking_at);

    this->lower_left_corner = this->origin - (half_width * focus_distance * this->u) -
        (half_height * focus_distance * this->v) - (focus_distance * this->w);
    this->horizontal = 2.f * half_width * focus_distance * u;
    this->vertical = 2.f * half_height * focus_distance * v;
}