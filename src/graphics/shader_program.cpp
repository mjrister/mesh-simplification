#include "graphics/shader_program.h"

#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace gfx {

namespace {

/**
 * @brief Retrieves the contents of a file.
 * @param filepath The filepath to load contents from.
 * @return A string containing the file contents.
 */
std::string ReadFile(const std::filesystem::path& filepath) {
  if (std::ifstream ifs{filepath, std::ios::ate | std::ios::binary}) {
    const std::streamsize size = ifs.tellg();
    std::string source(static_cast<std::size_t>(size), '\0');
    ifs.seekg(0, std::ios::beg);
    ifs.read(source.data(), size);
    return source;
  }
  throw std::runtime_error{std::format("Failed to open {}", filepath.generic_string())};
}

/**
 * @brief Verifies the status of a shader.
 * @param shader_id The shader ID.
 * @param status_type The status type to verify.
 */
void VerifyShaderStatus(const GLuint shader_id, const GLenum status_type) {
  GLint success{};
  glGetShaderiv(shader_id, status_type, &success);

  if (success == GL_FALSE) {
    GLsizei log_length{};
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<GLchar> info_log(log_length);
    glGetShaderInfoLog(shader_id, log_length, &log_length, info_log.data());
    throw std::runtime_error{info_log.data()};
  }
}

/**
 * @brief Verifies the status of a shader program.
 * @param shader_program_id The shader program ID.
 * @param status_type The shader program status type to verify.
 */
void VerifyShaderProgramStatus(const GLuint shader_program_id, const GLenum status_type) {
  GLint success{};
  glGetProgramiv(shader_program_id, status_type, &success);

  if (success == GL_FALSE) {
    GLsizei log_length{};
    glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<GLchar> info_log(log_length);
    glGetProgramInfoLog(shader_program_id, log_length, &log_length, info_log.data());
    throw std::runtime_error{info_log.data()};
  }
}

}  // namespace

ShaderProgram::Shader::Shader(const GLenum shader_type, const std::string& shader_source)
    : id{glCreateShader(shader_type)} {
  if (id == 0) throw std::runtime_error{"Shader creation failed"};

  const auto* shader_source_data = shader_source.data();
  glShaderSource(id, 1, &shader_source_data, nullptr);

  glCompileShader(id);
  VerifyShaderStatus(id, GL_COMPILE_STATUS);
}

ShaderProgram::ShaderProgram(const std::filesystem::path& vertex_shader_filepath,
                             const std::filesystem::path& fragment_shader_filepath)
    : id_{glCreateProgram()},
      vertex_shader_{GL_VERTEX_SHADER, ReadFile(vertex_shader_filepath)},
      fragment_shader_{GL_FRAGMENT_SHADER, ReadFile(fragment_shader_filepath)} {
  if (id_ == 0) throw std::runtime_error{"Shader program creation failed"};

  glAttachShader(id_, vertex_shader_.id);
  glAttachShader(id_, fragment_shader_.id);

  glLinkProgram(id_);
  VerifyShaderProgramStatus(id_, GL_LINK_STATUS);

  glValidateProgram(id_);
  VerifyShaderProgramStatus(id_, GL_VALIDATE_STATUS);

  glDetachShader(id_, vertex_shader_.id);
  glDetachShader(id_, fragment_shader_.id);
}

GLint ShaderProgram::GetUniformLocation(const std::string_view name) const {
  auto iterator = uniform_locations_.find(name);
  if (iterator == uniform_locations_.end()) {
    std::string name_string{name};
    const auto location = glGetUniformLocation(id_, name_string.c_str());
    if (static constexpr auto kNotActiveUniformVariable = -1; location == kNotActiveUniformVariable) {
      std::cerr << std::format("{} is not an active uniform variable\n", name);
      return kNotActiveUniformVariable;
    }
    iterator = uniform_locations_.emplace(std::move(name_string), location).first;
  }
  return iterator->second;
}

}  // namespace gfx
