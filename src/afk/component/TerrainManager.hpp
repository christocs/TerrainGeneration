#pragma once

#include "afk/renderer/Model.hpp"
#include <filesystem>
#include "afk/physics/Point.hpp"

namespace Afk {
    // using point = std::vector<unsigned>[2];
    class TerrainManager {
        public:
            auto generate_flat(unsigned width, unsigned height) -> void;

            auto generate_from_height_map(std::filesystem::path path, unsigned width, unsigned height) -> void;

            auto generate_fractal(unsigned width, unsigned height) -> void;

            auto get_model() -> Afk::Model;
        private:
            Afk::Model model;

            auto iterate_fractal(unsigned width, unsigned height, float displacement) -> void;

            auto normalise_height() -> void;

            auto normalise_xz_plane() -> void;

            auto get_random_coord(unsigned width, unsigned height) -> Afk::Point;
    };
};
