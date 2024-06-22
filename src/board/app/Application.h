#pragma once

#include "imgui.h"
#include "Layer.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <cassert>
#include <memory>
#include <vector>

class Application
{
public:
	struct Config
	{
		std::string title;
		int width;
		int height;
		bool resizable;
		bool fullscreen;
		bool enableDocking;
		std::string font;
	};

	enum class RenderBackend
	{
		Polling,
		WaitEvents,
	};

	static std::shared_ptr<Application> create(
		const Application::Config &config,
		Application::RenderBackend renderBackend = Application::RenderBackend::Polling);
	~Application();

	Application &addLayer(const std::shared_ptr<Layer> &layer);

	template <typename T>
	std::shared_ptr<T> getLayer(const std::string &name);

	Application &setWindowSize(int width, int height);
	GLFWwindow* window() const { return m_window; }
	void run();

private:
	explicit Application(const Application::Config &config, Application::RenderBackend renderBackend);
	void Init();
	void Cleanup();

private:
	/// OpenGL3 window data.
	GLFWwindow* m_window;

	/// Configuration of the GLFW window.
	Config m_config;

	/// @c RenderBackend enum variable identifying the rendering method.
	RenderBackend m_renderBackend;

	/// Flag to show the ImGui metrics window.
	ImVec4 m_clearColor;

	/// All the windows displayed in the application.
	std::vector<std::shared_ptr<Layer>> m_layers;
};

template <typename T>
std::shared_ptr<T> Application::getLayer(const std::string &name)
{
	static_assert(std::is_base_of<Layer, T>::value == true);

	for (auto &layer : m_layers) {
		if (layer->getName() == name) {
			return std::static_pointer_cast<T>(layer);
		}
	}

	return nullptr;
}

