#include "afk/component/TerrainManager.hpp"

auto Afk::TerrainManager::create_flat_plain(unsigned width, unsigned height) -> void {
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
