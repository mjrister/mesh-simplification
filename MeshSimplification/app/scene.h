#pragma once

#include "app/window.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace app {

/** \brief Defines all of the necessary objects (e.g., camera, lighting, mesh, etc.) to render the scene. */
class Scene {

public:
	/**
	 * \brief Initializes the scene.
	 * \param window The window to render to.
	 * \param shader_program The shader shader program to use for rendering.
	 */
	Scene(Window* window, gfx::ShaderProgram* shader_program);

	/**
	 * \brief Renders the scene.
	 * \param delta_time The amount of time elapsed since the last frame was rendered.
	 */
	void Render(float delta_time);

private:
	Window* window_;
	gfx::ShaderProgram* shader_program_;
	gfx::Mesh mesh_;
};
}
