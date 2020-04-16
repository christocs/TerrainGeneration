#include "afk/renderer/Camera.hpp"

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using glm::mat4;
using glm::vec3;

using Afk::Camera;

auto Camera::handle_mouse(float dx, float dy) -> void {
  constexpr auto max_yaw = 89.0f;

  this->angles.x += dx * this->sensitivity;
  this->angles.y += -dy * this->sensitivity;
  this->angles.y = std::clamp(this->angles.y, -max_yaw, max_yaw);
}

auto Camera::handle_key(Movement movement, float dt) -> void {
  const auto velocity = this->speed * dt;

  switch (movement) {
    case Movement::Forward: {
      this->position += this->get_front() * velocity;
    } break;
    case Movement::Backward: {
      this->position -= this->get_front() * velocity;
    } break;
    case Movement::Left: {
      this->position -= this->get_right() * velocity;
    } break;
    case Movement::Right: {
      this->position += this->get_right() * velocity;
    } break;
  }
}

auto Camera::get_view_matrix() -> mat4 {
  return glm::lookAt(this->position, this->position + this->get_front(), this->get_up());
}

auto Camera::get_projection_matrix(int width, int height) const -> mat4 {
  const auto w = static_cast<float>(width);
  const auto h = static_cast<float>(height);

  return glm::perspective(glm::radians(this->fov), w / h, this->near, this->far);
}

auto Camera::get_front() const -> vec3 {
  auto front = vec3{};

  front.x = std::cos(glm::radians(this->angles.x)) *
            std::cos(glm::radians(this->angles.y));
  front.y = std::sin(glm::radians(this->angles.y));
  front.z = std::sin(glm::radians(this->angles.x)) *
            std::cos(glm::radians(this->angles.y));

  front = glm::normalize(front);

  return front;
}

auto Camera::get_right() const -> vec3 {
  return glm::normalize(glm::cross(this->get_front(), this->WORLD_UP));
}

auto Camera::get_up() const -> vec3 {
  return glm::normalize(glm::cross(this->get_right(), this->get_front()));
}
