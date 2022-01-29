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
	Window* window_;
	gfx::ShaderProgram* shader_program_;
	gfx::Mesh mesh_;
};
}
