#include "afk/component/TerrainManager.hpp"

#include "afk/io/Path.hpp"
#include <iostream>
#include <fstream>
#include "afk/debug/Assert.hpp"
#include <string>

auto Afk::TerrainManager::generate_flat(unsigned width, unsigned height) -> void {
    this->model.meshes.clear();
    this->model.meshes.resize(1);

    std::vector<glm::vec2> texCoords;
    texCoords.resize(3);
    texCoords[0] = glm::vec2{0.0f, 0.0f};
    texCoords[1] = glm::vec2{1.0f, 0.0f};
    texCoords[2] = glm::vec2{0.5f, 1.0f};

    // define vertices for plane
    const auto no_vertices = static_cast<size_t>(width * height);
    this->model.meshes[0].vertices.resize(no_vertices);

    size_t vertex_index = 0;
    
    unsigned texCoordsIndex = 0;

    for (unsigned x = 0; x < width; x++) {
      for (unsigned z = 0; z < height; z++) {
        // this->mesh.vertices[vertex_index].position =
        //   glm::vec3{static_cast<float>(x), static_cast<float>((rand() % 20)*0.2f), static_cast<float>(z)};
        this->model.meshes[0].vertices[vertex_index].uvs = texCoords[texCoordsIndex++];
        //this->model.meshes[0].vertices[vertex_index].uvs = texCoords[rand() % 3];

        if (texCoordsIndex > 2) {
          texCoordsIndex = 0;
        }

        this->model.meshes[0].vertices[vertex_index].position =
          glm::vec3{static_cast<float>(x), 0.0f, static_cast<float>(z)};

        vertex_index++;
      }
    }

    // link vertices together
    size_t index_index = 0;
    this->model.meshes[0].indices.resize((width -1) * (height - 1) * 6);
    
    for (unsigned x = 0; x < (width - 1); x++) {
      for (unsigned z = 0; z < (height - 1); z++) {
        // vertex index = (z * width) + x
        auto base_index = static_cast<Mesh::Index>(x * height + z);

        this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index);
        this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index + height);
        this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index + height + 1);

        this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index);
        this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index + 1);
        this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index + height + 1);
      }
    }

    // file path is used as an identifier, so requires a unique identifier
    model.file_path = std::to_string(clock());

    this->normalise_xz_plane();

    // set texture
    // const auto abs_path = Afk::get_absolute_path(file_path);
    // auto importer       = Assimp::Importer{};

    // const auto *scene = importer.ReadFile(abs_path.string(), 0);
    // auto assimp_path = aiString{};
    //const auto a = ASSIMP_API;
    //aiMaterial::GetTexture(Texture::Type::Diffuse, 0, &assimp_path);
    // const auto file_path = get_texture_path(path{string{assimp_path.C_Str()}});

    // terrain_model.meshes[0].textures.resize(0);
    // terrain_model.meshes[0].textures[0].type = Afk::Texture::Type::Diffuse;
    // terrain_model.meshes[0].textures[0].file_path = "res/model/basketball/textures/Basketball.png";

}

auto Afk::TerrainManager::get_model() -> Afk::Model {
    return this->model;
}

auto Afk::TerrainManager::generate_from_height_map(std::filesystem::path path, unsigned width, unsigned height) -> void {
    afk_assert(width > 0, "Width cannot be 0");
    afk_assert(height > 0, "Height cannot be 0");

    this->generate_flat(width, height);

    auto abs = Afk::get_absolute_path(path);

    std::ifstream file (abs, std::ifstream::binary);

    afk_assert(!!file, "Failed to open filestream");

    file.seekg(0, file.end);
    auto length = static_cast<unsigned>(file.tellg());

    afk_assert(length == width * height, "Invalid file size");

    file.seekg(0, file.beg);

    char* buffer = new char [width];

    auto vertexIndex = std::size_t{0};
    while (file.read(buffer, width)) {
        for (unsigned i = 0; i < width; i++) {
            this->model.meshes[0].vertices[vertexIndex++].position.y = static_cast<float>(buffer[i]);
        }
    }

    file.close();

    delete[] buffer;

    this->normalise_height();
}

auto Afk::TerrainManager::generate_fractal(unsigned width, unsigned height) -> void {
    this->generate_flat(width, height);
    for (int i = 0; i < 800; i++) {
        this->iterate_fractal(width, height, 2.0f);
    }
    this->normalise_height();
}

auto Afk::TerrainManager::iterate_fractal(unsigned width, unsigned height, float displacement) -> void {
    // get two random points which are not the same
    auto point1 = this->get_random_coord(width, height);
    auto point2 = this->get_random_coord(width, height);
    while (point1.x == point2.x && point1.y == point2.y) {
        point2 = this->get_random_coord(width, height);
    }

    for (unsigned z = 0; z < height; z++) {
        for (unsigned x = 0; x < width; x++) {
            auto pointSide = (point2.x - point1.x) * (z - point1.y) - (point2.y - point1.y) * (x - point1.x);
            auto i = static_cast<size_t>(z * width + x);
            if (pointSide > 0) {
                this->model.meshes[0].vertices[i].position.y += static_cast<float>(displacement);
            } else if (pointSide < 0) {
                this->model.meshes[0].vertices[i].position.y -= static_cast<float>(displacement);
            }
        }
    }
}

auto Afk::TerrainManager::get_random_coord(unsigned width, unsigned height) -> Afk::Point {
    return Afk::Point{
        static_cast<unsigned>(rand() % static_cast<int>(width)),
        static_cast<unsigned>(rand() % static_cast<int>(height))
    };
}

auto Afk::TerrainManager::normalise_height() -> void {
    float maxHeight = 0;
    float minHeight = 0;

    float yTotal = 0;

    for (auto vertex : this->model.meshes[0].vertices) {
        if (vertex.position.y > maxHeight) {
            maxHeight = vertex.position.y;
        } else if (vertex.position.y < minHeight) {
            minHeight = vertex.position.y;
        }
        yTotal += vertex.position.y;
    }

    const auto range = maxHeight - minHeight;

    if (range > 0.01f || range < -0.01f) {
        for (auto& vertex : this->model.meshes[0].vertices) {
            // centre the position
            vertex.position.y = vertex.position.y - maxHeight - (yTotal / static_cast<float>(this->model.meshes[0].vertices.size()));
            //normalise the range
            vertex.position.y /= range;
        }
    }
}

auto Afk::TerrainManager::normalise_xz_plane() -> void {
    float maxX = 0;
    float minX = 0;
    float totalX = 0;

    float maxZ = 0;
    float minZ = 0;
    float totalZ = 0;

    for (auto vertex : this->model.meshes[0].vertices) {
        if (vertex.position.x > maxX) {
            maxX = vertex.position.x;
        } else if (vertex.position.x < minX) {
            minX = vertex.position.x;
        }
        totalX += vertex.position.x;

        if (vertex.position.z > maxZ) {
            maxZ = vertex.position.z;
        } else if (vertex.position.z < minZ) {
            minZ = vertex.position.z;
        }
        totalZ += vertex.position.z;
    }

    const auto rangeX = maxX - minX;
    const auto rangeZ = maxZ - minZ;

    for (auto& vertex : this->model.meshes[0].vertices) {
        // centre the position
        vertex.position.x = vertex.position.x - maxX - (totalX / static_cast<float>(this->model.meshes[0].vertices.size()));
        vertex.position.z = vertex.position.z - maxZ - (totalZ / static_cast<float>(this->model.meshes[0].vertices.size()));
        //normalise the range
        vertex.position.x /= rangeX;
        vertex.position.z /= rangeZ;
    }
}
