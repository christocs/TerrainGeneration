#include "afk/component/TerrainManager.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <random>

#include "afk/debug/Assert.hpp"
#include "afk/io/Path.hpp"

auto Afk::TerrainManager::generate_flat(unsigned width, unsigned height) -> void {
  this->model.meshes.clear();
  this->model.meshes.resize(1);

  std::vector<glm::vec2> texCoords;
  texCoords.resize(3);
  texCoords[0] = glm::vec2{0.0f, 0.0f};
  texCoords[1] = glm::vec2{1.0f, 0.0f};
  texCoords[2] = glm::vec2{0.5f, 1.0f};

  // define vertices for plane
  const auto no_vertices = width * height;
  this->model.meshes[0].vertices.resize(static_cast<size_t>(no_vertices));

  size_t vertex_index = 0;

  unsigned texCoordsIndex = 0;
  bool ascending          = true;

  for (unsigned x = 0; x < width; x++) {
    for (unsigned z = 0; z < height; z++) {
      this->model.meshes[0].vertices[vertex_index].uvs = texCoords[texCoordsIndex];

      if (ascending) {
        texCoordsIndex++;
        if (texCoordsIndex == (texCoords.size() - 1)) {
          ascending = !ascending;
        }
      } else {
        texCoordsIndex--;
        if (texCoordsIndex == 0) {
          ascending = !ascending;
        }
      }

      this->model.meshes[0].vertices[vertex_index].position =
          glm::vec3{static_cast<float>(x), 0.0f, static_cast<float>(z)};

      vertex_index++;
    }
  }

  // link vertices together
  size_t index_index = 0;
  this->model.meshes[0].indices.resize((width - 1) * (height - 1) * 6);

  for (unsigned x = 0; x < (width - 1); x++) {
    for (unsigned z = 0; z < (height - 1); z++) {
      // vertex index = (z * width) + x
      auto base_index = static_cast<Mesh::Index>(x * height + z);

      this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + height);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + height + 1);

      this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + 1);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + height + 1);
    }
  }

  // file path is used as an identifier, so requires a unique identifier
  model.file_path = std::to_string(clock());

  this->normalise_xz_plane();
}

auto Afk::TerrainManager::get_model() -> Afk::Model {
  this->assign_textures();
  return this->model;
}

auto Afk::TerrainManager::generate_from_height_map(const std::filesystem::path& path, unsigned width,
                                                   unsigned height) -> void {
  afk_assert(width > 0, "Width cannot be 0");
  afk_assert(height > 0, "Height cannot be 0");

  this->generate_flat(width, height);

  auto abs = Afk::get_absolute_path(path);

  std::ifstream file(abs, std::ifstream::binary);

  afk_assert(!!file, "Failed to open filestream");

  file.seekg(0, std::ifstream::end);
  auto length = static_cast<unsigned>(file.tellg());

  afk_assert(length == width * height, "Invalid file size");

  file.seekg(0, std::ifstream::beg);

  char *buffer = new char[width];

  auto vertexIndex = std::size_t{0};
  while (file.read(buffer, width)) {
    for (unsigned i = 0; i < width; i++) {
      auto value = static_cast<float>(buffer[i]);
      if (value < 0) {
        value = -value;
      }
      this->model.meshes[0].vertices[vertexIndex++].position.y = value;
    }
  }

  file.close();

  delete[] buffer;

  this->normalise_height();
}

auto Afk::TerrainManager::generate_fractal(unsigned width, unsigned height) -> void {
  this->generate_flat(width, height);
  // const auto noIterations = width * height;
  const auto noIterations = 1;
  for (int i = 0; i < noIterations; i++) {
    this->iterate_fractal(width, height,
                          noIterations - (static_cast<float>(i) / noIterations));
  }
  this->normalise_height();
}

auto Afk::TerrainManager::iterate_fractal(unsigned width, unsigned height,
                                          float displacement) -> void {
  // get two random points which are not the same
  auto point1 = Afk::TerrainManager::get_random_coord(width, height);
  auto point2 = Afk::TerrainManager::get_random_coord(width, height);
  while (point1.x == point2.x && point1.y == point2.y) {
    point2 = Afk::TerrainManager::get_random_coord(width, height);
  }

  for (unsigned x = 0; x < width; x++) {
    const auto stride = x * height;
    for (unsigned z = 0; z < height; z++) {
      auto pointSide = ((x - point1.x) * (point2.y - point1.y)) -
                       ((z - point1.y) * (point2.x - point1.x));
      const auto i = stride + z;
      if (pointSide > 0) {
        this->model.meshes[0].vertices[i].position.y += static_cast<float>(displacement);
      }
    }
  }
}

auto Afk::TerrainManager::get_random_coord(unsigned width, unsigned height) -> Afk::Point {
  return Afk::Point{static_cast<unsigned>(random() % static_cast<int>(width - 1)),
                    static_cast<unsigned>(random() % static_cast<int>(height - 1))};
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
    for (auto &vertex : this->model.meshes[0].vertices) {
      // centre the position
      vertex.position.y =
          vertex.position.y - maxHeight -
          (yTotal / static_cast<float>(this->model.meshes[0].vertices.size()));
      // normalise the range
      vertex.position.y /= range;
    }
  }
}

auto Afk::TerrainManager::normalise_xz_plane() -> void {
  float maxX   = 0;
  float minX   = 0;
  float totalX = 0;

  float maxZ   = 0;
  float minZ   = 0;
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

  for (auto &vertex : this->model.meshes[0].vertices) {
    // centre the position
    vertex.position.x =
        vertex.position.x - maxX -
        (totalX / static_cast<float>(this->model.meshes[0].vertices.size()));
    vertex.position.z =
        vertex.position.z - maxZ -
        (totalZ / static_cast<float>(this->model.meshes[0].vertices.size()));
    // normalise the range
    vertex.position.x /= rangeX;
    vertex.position.z /= rangeZ;
  }
}

auto Afk::TerrainManager::assign_textures() -> void {
  for (auto &mesh : this->model.meshes) {
    mesh.textures.push_back(Afk::Texture("res/texture/grass.png"));
  }
}
