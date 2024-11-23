#pragma once

#include <string>
#include <memory>

#include "imgui.h"

class Application;

class Layer
{
public:
	explicit Layer(const std::string &name, const ImGuiWindowFlags &windowFlags = ImGuiWindowFlags_None)
		: m_windowFlags(windowFlags)
		, m_name(name)
	{
		m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
		m_windowFlags |= ImGuiWindowFlags_NoDecoration;
		m_windowFlags |= ImGuiWindowFlags_NoCollapse;
		m_windowFlags |= ImGuiWindowFlags_NoScrollbar;
		m_windowFlags |= ImGuiWindowFlags_NoMove;
		m_windowFlags |= ImGuiWindowFlags_NoResize;
	}

	virtual ~Layer() {}

	virtual void onAttach() {}
	virtual void onDetach() {}

	std::string name() const { return m_name; }

	virtual void render() {}

	Application &app() { return *m_app.lock(); }

	void setApp(const std::shared_ptr<Application> &app) { m_app = app; }

protected:
	ImGuiWindowFlags m_windowFlags;

	// Name of the layer is used for identification in the LayerStack.
	std::string m_name;

	// This needs to be a weak_ptr to avoid circular references
	// and to ensure that the Application object is destroyed
	// and subsequently all its layers
	std::weak_ptr<Application> m_app;
};
