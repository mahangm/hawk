#include "../system/core.hpp"
#include "renderer.hpp"
#include "../objects/entity.hpp"
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace hawk
{
	namespace Components
	{
		Renderer::Renderer() : onRender(nullptr), shadows(Shadows::On), cast_shadows(true), mesh(nullptr), mode(GL_FILL)
		{
			// set component type
			m_type = "Renderer";
		}

		Renderer::Renderer(const Renderer& other) : Component(other), onRender(other.onRender), 
			shadows(other.shadows), cast_shadows(other.cast_shadows), materials(other.materials), mesh(other.mesh), mode(other.mode)
		{
			// set component type
			m_type = "Renderer";
		}

		Renderer::~Renderer()
		{
			// remove from render
			removeFromVideoRenderers();
		}

		Renderer* Renderer::copy()
		{
			return new Renderer(*this);
		}

		void Renderer::operator=(const Renderer& other)
		{
			onRender = other.onRender;
			shadows = other.shadows;
			cast_shadows = other.cast_shadows;
			materials = other.materials;
			mesh = other.mesh;
			mode = other.mode;
		}

		void Renderer::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;
		}

		void Renderer::release()
		{
			// free object
			delete this;
		}

		void Renderer::render() {}

		void Renderer::render(Assets::Material* material, Assets::Mesh* mesh) {}

		void Renderer::render(Assets::Material* material, const Light* light) {}

		void Renderer::destroy()
		{
			// remove mesh renderers from render 
			removeFromVideoRenderers();
		}

		void Renderer::addToVideoRenderers()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// add to mesh renderers list
			app.video.renderers.push_back(this);
		}

		void Renderer::removeFromVideoRenderers()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// mesh renderer iterator
			auto iterator = std::find(app.video.renderers.begin(), app.video.renderers.end(), this);

			// remove from mesh renderers list
			if (iterator != app.video.renderers.end())
				app.video.renderers.erase(iterator);
		}
	}
}