#pragma once

#include <concepts>
#include <filesystem>
#include <format>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

namespace gfx {

/** \brief A program used to run one or more OpenGL shaders on the host GPU. */
class ShaderProgram {
  /** \brief A shader in the OpenGL graphics pipeline. */
  class Shader {
  public:
    /**
     * \brief Creates a shader.
     * \param shader_type The shader type (e.g., GL_FRAGMENT_SHADER)
     * \param shader_source The shader source code to be compiled.
     * \throw std::runtime_error Thrown if shader creation was unsuccessful.
     */
    Shader(GLenum shader_type, const std::string& shader_source);

    ~Shader() noexcept { glDeleteShader(id); }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&&) noexcept = delete;
    Shader& operator=(Shader&&) noexcept = delete;

    GLuint id;  // NOLINT(misc-non-private-member-variables-in-classes): allow direct access to internal shader class
  };

public:
  /**
   * \brief Creates a shader program.
   * \param vertex_shader_filepath The filepath to the vertex shader to be compiled.
   * \param fragment_shader_filepath The filepath to the fragment shader to be compiled.
   * \throw std::runtime_error Thrown if the a file could not be opened or shader program creation was unsuccessful.
   */
  ShaderProgram(const std::filesystem::path& vertex_shader_filepath,
                const std::filesystem::path& fragment_shader_filepath);

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;

  ShaderProgram(ShaderProgram&&) noexcept = delete;
  ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

  ~ShaderProgram() noexcept { glDeleteProgram(id_); }

  /** \brief Binds the shader program to the current OpenGL context. */
  void Enable() const noexcept { glUseProgram(id_); }

  /**
   * \brief Sets a uniform variable in the shader program.
   * \tparam T The uniform variable type.
   * \param name The uniform variable name.
   * \param value The uniform variable value.
   */
  template <typename T>
  void SetUniform(const std::string_view name, const T& value) const {
    const auto location = GetUniformLocation(name);

    if constexpr (std::integral<T>) {
      glUniform1i(location, static_cast<GLint>(value));
    } else if constexpr (std::floating_point<T>) {
      glUniform1f(location, static_cast<GLfloat>(value));
    } else if constexpr (std::same_as<T, glm::vec3>) {
      glUniform3fv(location, 1, glm::value_ptr(value));
    } else if constexpr (std::same_as<T, glm::vec4>) {
      glUniform4fv(location, 1, glm::value_ptr(value));
    } else if constexpr (std::same_as<T, glm::mat3>) {
      glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    } else if constexpr (std::same_as<T, glm::mat4>) {
      glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    } else {
      static_assert(kAssertFalse<T>, "Unsupported uniform variable type");
    }
  }

private:
  // this is required as a workaround to ensure that static assertions in "if constexpr" expressions are well-formed
  template <typename>
  static constexpr std::false_type kAssertFalse{};

  // The following is needed to perform heterogeneous lookup in unordered containers. This is important because
  // each uniform location query is performed using a string_view, but stored as a string. Without heterogeneous
  // lookup, each query would have to be converted to a string (and hence allocate unnecessary memory) which would
  // degrade performance on the critical rendering path.
  struct StringViewHash {
    using is_transparent = void;
    static constexpr std::hash<std::string_view> kStringViewHash;

    std::size_t operator()(const std::string_view value) const noexcept { return kStringViewHash(value); }
  };

  /**
   * \brief Gets the location for a uniform variable in the shader program.
   * \param name The uniform variable name.
   * \return An integer representing the uniform variable location. Returns -1 if the variable is not active.
   */
  [[nodiscard]] GLint GetUniformLocation(const std::string_view name) const {
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

  GLuint id_;
  Shader vertex_shader_, fragment_shader_;
  mutable std::unordered_map<std::string, GLint, StringViewHash, std::equal_to<>> uniform_locations_;
};

}  // namespace gfx
