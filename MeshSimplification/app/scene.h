#pragma once

#include "app/window.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace app {

class Scene {

public:
	Scene(Window* window, gfx::ShaderProgram* shader_program);
	void Render(float delta_time);

private:
	void InitializeMesh();
	void InitializePointLights();

	void UpdateProjectionTransform();

	void HandleDiscreteKeyPress(int key_code);
	void HandleContinuousInput(float delta_time);

	Window* const window_;
	gfx::ShaderProgram* const shader_program_;
	gfx::Mesh mesh_;
};
}
