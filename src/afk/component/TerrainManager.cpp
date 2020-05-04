#include "afk/component/TerrainManager.hpp"

#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "afk/debug/Assert.hpp"
#include "afk/io/Path.hpp"

auto Afk::TerrainManager::generate_flat(unsigned width, unsigned length) -> void {
  this->model.meshes.clear();
  this->model.meshes.resize(1);

  std::vector<glm::vec2> texCoords;
  texCoords.resize(3);
  texCoords[0] = glm::vec2{0.0f, 0.0f};
  texCoords[1] = glm::vec2{1.0f, 0.0f};
  texCoords[2] = glm::vec2{0.5f, 1.0f};

  // define vertices for plane
  const auto no_vertices = width * length;
  this->model.meshes[0].vertices.resize(static_cast<size_t>(no_vertices));

  size_t vertex_index = 0;

  unsigned texCoordsIndex = 0;
  // determines if the indexes of UVs to use are ascending or descending
  bool ascending          = true;

  for (unsigned x = 0; x < width; x++) {
    for (unsigned z = 0; z < length; z++) {
      this->model.meshes[0].vertices[vertex_index].uvs = texCoords[texCoordsIndex];

      // increments or decrements the index of the height map
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
  this->model.meshes[0].indices.resize((width - 1) * (length - 1) * 6);

  for (unsigned x = 0; x < (width - 1); x++) {
    for (unsigned z = 0; z < (length - 1); z++) {
      // vertex index = (z * width) + x
      auto base_index = static_cast<Mesh::Index>(x * length + z);

      this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + length);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + length + 1);

      this->model.meshes[0].indices[index_index++] = static_cast<Mesh::Index>(base_index);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + 1);
      this->model.meshes[0].indices[index_index++] =
          static_cast<Mesh::Index>(base_index + length + 1);
    }
  }

  this->normalise_xz_plane();
}

auto Afk::TerrainManager::get_model(const std::filesystem::path &virtual_file_path) -> Afk::Model {
  this->model.file_path = virtual_file_path;
  this->model.file_dir = virtual_file_path.parent_path();
  this->assign_textures("res/texture/grass.png");
  return this->model;
}

auto Afk::TerrainManager::generate_from_height_map(const std::filesystem::path &path,
                                                   unsigned width, unsigned length) -> void {
  afk_assert(width > 0, "Width cannot be 0");
  afk_assert(length > 0, "length cannot be 0");

  this->generate_flat(width, length);

  auto abs = Afk::get_absolute_path(path);

  std::ifstream file(abs, std::ifstream::binary);

  afk_assert(!!file, "Failed to open filestream");

  file.seekg(0, std::ifstream::end);
  auto file_length = static_cast<unsigned>(file.tellg());

  afk_assert(file_length == width * length, "Invalid file size");

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

// diamond square algorithm
// @see https://medium.com/@nickobrien/diamond-square-algorithm-explanation-and-c-implementation-5efa891e486f
// TODO: support rectangular height maps
auto Afk::TerrainManager::generate_fractal(unsigned width, unsigned length) -> void {
  afk_assert(width == length, "fractal height maps must be square");
  this->generate_flat(width, length);

  unsigned int size = length;
  unsigned int half = size / 2;
  while (half > 0) {
    // square steps
    for (auto z = half; z < length; z += size) {
      for (auto x = half; x < length; x += size) {
        this->fractal_square_step(length, x % width, z % width, half);
      }
    }

    // diamond steps
    unsigned int col = 0;
    for (unsigned int x = 0; x < width; x += half) {
      col++;
      if (col % 2 == 1) {
        for (unsigned int z = half; z < length; z += size) {
          this->fractal_diamond_step(length, x % width, z % length, half);
        }
      } else {
        for (unsigned int z = 0; z < length; z += size) {
          this->fractal_diamond_step(length, x % width, z % length, half);
        }
      }
    }

    size = half;
    half = size / 2;
  }

  this->normalise_height();
}

auto Afk::TerrainManager::fractal_square_step(unsigned height, unsigned x,
                                              unsigned z, unsigned reach) -> void {
  unsigned int count = 0;
  float avg          = 0.0f;
  if (((static_cast<int>(x) - static_cast<int>(reach)) >= 0) && ((static_cast<int>(z) - static_cast<int>(reach)) >= 0)) {
    avg += this->get_height_at(height, x, z);
    count++;
  }
  if (((static_cast<int>(x) - static_cast<int>(reach)) >= 0) && ((z + reach) >= height)) {
    avg += this->get_height_at(height, x, z);
    count++;
  }
  if (((x + reach) < height) && ((static_cast<int>(z) - static_cast<int>(reach)) >= 0)) {
    avg += this->get_height_at(height, x, z);
    count++;
  }
  if (((x + reach) < height) && ((z + reach) < height)) {
    avg += this->get_height_at(height, x, z);
    count++;
  }

  avg += static_cast<float>(Afk::TerrainManager::range_random(static_cast<int>(reach)));
  avg /= static_cast<float>(count);
  this->model.meshes[0]
      .vertices[Afk::TerrainManager::get_index(height, x, z)]
      .position.y = avg;
}

auto Afk::TerrainManager::fractal_diamond_step(unsigned height, unsigned x,
                                              unsigned z, unsigned reach) -> void {
  unsigned int count = 0;
  float avg          = 0.0f;

  if ((static_cast<int>(x) - static_cast<int>(reach)) >= 0) {
    avg += this->get_height_at(height, x - reach, z);
    count++;
  }
  if ((x + reach) < height) {
    avg += this->get_height_at(height, x + reach, z);
    count++;
  }
  if ((static_cast<int>(z) - static_cast<int>(reach)) >= 0) {
    avg += this->get_height_at(height, x, z - reach);
    count++;
  }
  if ((z + reach) < height) {
    avg += this->get_height_at(height, x, z + reach);
    count++;
  }

  avg += static_cast<float>(Afk::TerrainManager::range_random(static_cast<int>(reach)));
  avg /= static_cast<float>(count);
  this->model.meshes[0]
      .vertices[Afk::TerrainManager::get_index(height, x, z)]
      .position.y = avg;
}

auto Afk::TerrainManager::get_random_coord(unsigned width, unsigned length) -> Afk::Point {
  return Afk::Point{static_cast<unsigned>(random() % static_cast<int>(width - 1)),
                    static_cast<unsigned>(random() % static_cast<int>(length - 1))};
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

auto Afk::TerrainManager::assign_textures(const std::filesystem::path &texture_path) -> void {
  for (auto &mesh : this->model.meshes) {
    mesh.textures.push_back(Afk::Texture(texture_path));
  }
}

auto Afk::TerrainManager::get_index(unsigned map_height, unsigned x, unsigned y) -> unsigned {
  return map_height * y + x;
}

auto Afk::TerrainManager::get_height_at(unsigned map_height, unsigned x, unsigned y) -> float {
  return this->model.meshes[0]
      .vertices[this->get_index(map_height, x, y)]
      .position.y;
}

auto Afk::TerrainManager::range_random(int range) -> int {
  return static_cast<int>((random() % (range * 2)) - range) * 5;
}
