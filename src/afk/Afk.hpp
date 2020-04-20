#pragma once

#include <glm/glm.hpp>

#include "afk/event/EventManager.hpp"
#include "afk/renderer/Camera.hpp"
#include "afk/renderer/Renderer.hpp"
#include "afk/ui/Ui.hpp"
#include "afk/component/TerrainManager.hpp"

namespace Afk {
  class Engine {
  public:
    static constexpr const char *GAME_NAME = "ICT397";

    Renderer renderer          = {};
    Ui ui                      = {this->renderer.window};
    Camera camera              = {};
    EventManager event_manager = {};

    Engine();
    Engine(Engine &&)      = delete;
    Engine(const Engine &) = delete;
    auto operator=(const Engine &) -> Engine & = delete;
    auto operator=(Engine &&) -> Engine & = delete;

    static auto get() -> Engine &;

    auto render() -> void;
    auto update() -> void;

    auto get_time() -> float;
    auto get_delta_time() -> float;
    auto get_is_running() const -> bool;

  private:
    auto update_camera() -> void;
    auto move_keyboard(Event e) -> void;
    auto move_mouse(Event e) -> void;
    auto handle_mouse() -> void;

    bool is_running   = true;
    int frame_count   = {};
    float last_update = {};

    Afk::TerrainManager fractal_terrain_manager;
    Afk::ModelHandle fractal_terrain_model_handle;

    Afk::TerrainManager heightmap_terrain_manager;
    Afk::ModelHandle heightmap_terrain_model_handle;
  };
}
