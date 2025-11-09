#ifndef GRAPHICS_SHADER_PROGRAM_H_
#define GRAPHICS_SHADER_PROGRAM_H_

#include <concepts>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

namespace gfx {

/** @brief A program used to run one or more OpenGL shaders on the host GPU. */
class ShaderProgram {
public:
  /**
   * @brief Creates a shader program.
   * @param vertex_shader_filepath The filepath to the vertex shader to be compiled.
   * @param fragment_shader_filepath The filepath to the fragment shader to be compiled.
   * @throw std::runtime_error Thrown if the a file could not be opened or shader program creation was unsuccessful.
   */
  ShaderProgram(const std::filesystem::path& vertex_shader_filepath,
                const std::filesystem::path& fragment_shader_filepath);

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;

  ShaderProgram(ShaderProgram&&) noexcept = delete;
  ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

  ~ShaderProgram() noexcept { glDeleteProgram(id_); }

  /** @brief Binds the shader program to the current OpenGL context. */
  void Enable() const noexcept { glUseProgram(id_); }

  /**
   * @brief Sets a uniform variable in the shader program.
   * @tparam T The uniform variable type.
   * @param name The uniform variable name.
   * @param value The uniform variable value.
   */
  template <typename T>
  void SetUniform(const std::string_view name, const T& value) const {
    const auto uniform_location = GetUniformLocation(name);

    if constexpr (std::integral<T>) {
      glUniform1i(uniform_location, static_cast<GLint>(value));
    } else if constexpr (std::floating_point<T>) {
      glUniform1f(uniform_location, static_cast<GLfloat>(value));
    } else if constexpr (std::same_as<T, glm::vec3>) {
      glUniform3fv(uniform_location, 1, glm::value_ptr(value));
    } else if constexpr (std::same_as<T, glm::vec4>) {
      glUniform4fv(uniform_location, 1, glm::value_ptr(value));
    } else if constexpr (std::same_as<T, glm::mat3>) {
      glUniformMatrix3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
    } else if constexpr (std::same_as<T, glm::mat4>) {
      glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(value));
    } else {
      static_assert(kAssertFalse<T>, "Unsupported uniform variable type");
    }
  }

private:
  /** @brief A shader in the OpenGL graphics pipeline. */
  class Shader {
  public:
    /**
     * @brief Creates a shader.
     * @param shader_type The shader type (e.g., GL_FRAGMENT_SHADER)
     * @param shader_source The shader source code to be compiled.
     * @throw std::runtime_error Thrown if shader creation was unsuccessful.
     */
    Shader(GLenum shader_type, const std::string& shader_source);

    ~Shader() noexcept { glDeleteShader(id); }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&&) noexcept = delete;
    Shader& operator=(Shader&&) noexcept = delete;

    GLuint id;  // NOLINT(misc-non-private-member-variables-in-classes): allow direct access to internal shader class
  };

  // The following is needed to perform heterogeneous lookup in unordered containers. This is important because
  // each uniform location query is performed using a string_view, but stored as a string. Without heterogeneous
  // lookup, each query would have to be converted to a string (and hence allocate unnecessary memory) which would
  // degrade performance on the critical rendering path.
  struct StringViewHash {
    using is_transparent = void;
    static constexpr std::hash<std::string_view> kStringViewHash;

    std::size_t operator()(const std::string_view value) const noexcept { return kStringViewHash(value); }
  };

  // this is required as a workaround to ensure that static assertions in "if constexpr" expressions are well-formed
  template <typename>
  static constexpr std::false_type kAssertFalse{};

  /**
   * @brief Gets the location for a uniform variable in the shader program.
   * @param name The uniform variable name.
   * @return An integer representing the uniform variable location. Returns -1 if the variable is not active.
   */
  [[nodiscard]] GLint GetUniformLocation(std::string_view name) const;

  GLuint id_;
  Shader vertex_shader_, fragment_shader_;
  mutable std::unordered_map<std::string, GLint, StringViewHash, std::equal_to<>> uniform_locations_;
};

}  // namespace gfx

#endif  // GRAPHICS_SHADER_PROGRAM_H_
