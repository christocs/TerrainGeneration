#pragma once

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

class TerrainHeightMap {
    public:
        bool load(const std::string& fileName, int width, int height);
        void render();
    private:
        vector< vector<char> > heightMap;
};
