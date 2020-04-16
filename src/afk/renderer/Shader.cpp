#include "afk/renderer/Shader.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

#include "afk/debug/Assert.hpp"
#include "afk/io/Path.hpp"

using Afk::Shader;

using namespace std::string_literals;
using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::unordered_map;
using std::filesystem::path;
using Type = Shader::Type;

static auto shader_type_from_extension(const string &extension) -> Shader::Type {
  static const auto types = unordered_map<string, Type>{
      {".vert", Type::Vertex},
      {".frag", Type::Fragment},
  };

  return types.at(extension);
}

Shader::Shader(const path &_file_path) {
  const auto abs_path = Afk::get_absolute_path(_file_path);

  auto file = ifstream{abs_path};

  afk_assert(file.is_open(), "Unable to open shader '"s + abs_path.string() + "'"s);

  this->code =
      string{(istreambuf_iterator<char>(file)), istreambuf_iterator<char>()} + '\0';
  this->type      = shader_type_from_extension(_file_path.extension().string());
  this->file_path = _file_path;
}
