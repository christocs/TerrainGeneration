#pragma once

#include <filesystem>

#include "afk/physics/Point.hpp"
#include "afk/renderer/Model.hpp"

namespace Afk {
  // using point = std::vector<unsigned>[2];
  class TerrainManager {
  public:
    TerrainManager() = default;

    auto generate_flat(unsigned width, unsigned height) -> void;

    auto generate_from_height_map(const std::filesystem::path &path,
                                  unsigned width, unsigned height) -> void;

    auto generate_fractal(unsigned width, unsigned height) -> void;

    auto get_model(const std::filesystem::path &virtual_file_path) -> Afk::Model;

  private:
    Afk::Model model;

    auto assign_textures(const std::filesystem::path &texture_path) -> void;

    auto normalise_height() -> void;

    auto normalise_xz_plane() -> void;

    static auto get_random_coord(unsigned width, unsigned height) -> Afk::Point;

    static auto get_index(unsigned map_height, unsigned x, unsigned y) -> unsigned;

    auto get_height_at(unsigned map_height, unsigned x, unsigned y) -> float;

    auto fractal_square_step(unsigned height, unsigned width, unsigned x,
                             unsigned z, unsigned reach) -> void;

    auto fractal_diamond_step(unsigned height, unsigned width, unsigned x,
                             unsigned z, unsigned reach) -> void;

    static auto range_random(int range) -> int;
  };
};
