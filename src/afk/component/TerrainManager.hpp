#pragma once

#include "afk/renderer/Model.hpp"
#include <filesystem>

namespace Afk {
    class TerrainManager {
        public:
            auto generate_flat(unsigned width, unsigned height) -> void;

            auto generate_from_height_map(std::filesystem::path path, unsigned width, unsigned height) -> void;

            auto generate_fractal(unsigned width, unsigned height) -> void;

            auto get_model() -> Afk::Model;
        private:
            Afk::Model model;
    };
};
