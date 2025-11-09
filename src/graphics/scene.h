#ifndef GRAPHICS_SCENE_H_
#define GRAPHICS_SCENE_H_

#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace gfx {
class Window;

/** @brief Defines all of the necessary objects (e.g., camera, lighting, mesh, etc.) to render the scene. */
class Scene {
public:
  /**
   * @brief Creates a scene.
   * @param window The window to render to.
   */
  explicit Scene(Window* window);

  /**
   * @brief Renders a scene.
   * @param delta_time The amount of time elapsed since the last frame was rendered.
   */
  void Render(float delta_time);

private:
  Window* window_;
  Mesh mesh_;
  ShaderProgram shader_program_;
};

}  // namespace gfx

#endif  // GRAPHICS_SCENE_H_
