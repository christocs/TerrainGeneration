#pragma once

#include "afk/renderer/Model.hpp"

namespace Afk {
    class TerrainManager {
        public:
            auto create_flat_plain(unsigned width, unsigned height) -> void;

            auto get_model() -> Afk::Model;
        private:
            Afk::Model model;
    };
};
