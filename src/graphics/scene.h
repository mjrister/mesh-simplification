#ifndef GRAPHICS_SCENE_H_
#define GRAPHICS_SCENE_H_

#include <filesystem>

#include "graphics/arc_camera.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace gfx {
class Window;

class Scene {
public:
  Scene(const Window& window, const std::filesystem::path& obj_filepath);

  [[nodiscard]] ArcCamera& camera() noexcept { return camera_; }
  [[nodiscard]] Mesh& mesh() noexcept { return mesh_; }

  void Render() const;

private:
  ArcCamera camera_;
  Mesh mesh_;
  ShaderProgram shader_program_;
};

}  // namespace gfx

#endif  // GRAPHICS_SCENE_H_
