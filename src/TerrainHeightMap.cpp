#include "TerrainHeightMap.hpp"

#include <iostream>
#include <fstream>
#include <GL/gl.h>

bool TerrainHeightMap::load(const std::string& fileName, int width, int height)
{
    bool success = false;

    this->heightMap.clear();

    std::ifstream file (fileName, std::ifstream::binary);

    if (file) {
        file.seekg(0, file.end);
        long fileLength = file.tellg();
        file.seekg(0, file.beg);
        std::cout << "file length " << fileLength << std::endl;
        std::cout << "height map size " << width*height << std::endl;
        if (fileLength == (width * height)) {

            // read in horizontal lines one by one
            char* buffer = new char [width];
            for (int i = 0; i < height; i++) {
                file.read(buffer, width);

                vector<char> rowVector;
                for (int j = 0; j < width; j++) {
                    rowVector.push_back(buffer[j]);
                }
                this->heightMap.push_back(rowVector);
            }
            delete[] buffer;

            success = true;
        } else if (fileLength < (width * height)) {
            std::cerr << "Height map too large" << std::endl;
        } else {
            std::cerr << "Height map too small" << std::endl;
        }
    }

    return success;
}

void TerrainHeightMap::render()
{

    int temp = 300;
    int y = 0;
    glBegin(GL_POLYGON);
    glVertex3f(temp, 0, y);
    glVertex3f(0, temp, y);
    glVertex3f(temp, temp, y);
    glVertex3f(temp, 0, y);
    glEnd();

    temp = 100;
    y = 50;
    glBegin(GL_POLYGON);
    glVertex3f(temp, 0, y);
    glVertex3f(0, temp, y);
    glVertex3f(temp, temp, y);
    glVertex3f(temp, 0, y);
    glEnd();
    return;
    
    
    if (this->heightMap.size() > 0 && this->heightMap[0].size() > 0) {
        glColor3f(1, 0, 0);

        for (int i = 0; i < this->heightMap.size(); i++) {
            glBegin(GL_TRIANGLES);
            for (int j = 0; j < this->heightMap[i].size(); j++) {
                glVertex3f(i, i, j);
                glVertex3f(i, j, j+1);
            }
            glEnd();
        }
    }
    
}