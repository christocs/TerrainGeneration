#pragma once

#include <filesystem>

#include "afk/physics/Point.hpp"
#include "afk/renderer/Model.hpp"

namespace Afk {

  class TerrainManager {
  public:
    /**
     * Default constructor
     */
    TerrainManager() = default;

    /**
     * Generate flat terrain
     * @param width - width of terrain in the x direction
     * @param length - length of the terrain in the z direction
     */
    auto generate_flat(unsigned width, unsigned length) -> void;

    /**
     * Generate a terrain from height map
     * @param path - path of where to find the height map
     * @param width - width of the height map in the x direction
     * @param length - length of the height map in the z direction
     */
    auto generate_from_height_map(const std::filesystem::path &path,
                                  unsigned width, unsigned length) -> void;

    auto generate_fractal(unsigned width, unsigned length) -> void;

    auto get_model(const std::filesystem::path &virtual_file_path) -> Afk::Model;

  private:
    Afk::Model model;

    auto assign_textures(const std::filesystem::path &texture_path) -> void;

    auto normalise_height() -> void;

    auto normalise_xz_plane() -> void;

    static auto get_random_coord(unsigned width, unsigned length) -> Afk::Point;

    static auto get_index(unsigned map_height, unsigned x, unsigned y) -> unsigned;

    auto get_height_at(unsigned map_height, unsigned x, unsigned y) -> float;

    auto fractal_square_step(unsigned height, unsigned x,
                             unsigned z, unsigned reach) -> void;

    auto fractal_diamond_step(unsigned height, unsigned x,
                             unsigned z, unsigned reach) -> void;

    static auto range_random(int range) -> int;
  };
};
