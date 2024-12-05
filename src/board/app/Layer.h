#pragma once

#include <string>
#include <memory>

#include "imgui.h"

class Application;

/**
 * @class Layer
 * @brief The Layer class is the base class for all the layers in the application.
 *
 * The Layer class is an abstract class that needs to be inherited by all the layers in the application.
 * This class provides interface for the rendering the layers in the applcation. Also it allowes the user
 * to access other layers in the application.
 */
class Layer
{
public:
	/**
	 * @brief Constructor for the Layer class.
	 *
	 * The constructor initializes the Layer with the given name and ImGui flags.
	 * The ImGui flags are used to control the appearance of the window.
	 *
	 * @param name The name of the layer.
	 * @param windowFlags The ImGui flags used for the window.
	 */
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

	/// Virtual destructor to ensure that the derived classes are properly destroyed.
	virtual ~Layer() {}

	/// OnAttach method is called when the layer is added to the LayerStack.
	virtual void onAttach() {}
	/// OnDetach method is called when the layer is removed from the LayerStack.
	virtual void onDetach() {}

	std::string name() const { return m_name; }

	/**
	 * @brief The virtual render function needs to be overriten in child classes.
	 *
	 * The render function is responsible for rendering the design and handling the user input.
	 * The function is called by the Application class.
	 *
	 * @see Application The main class of the application.
	 */
	virtual void render() {}

	/**
	 * @brief Method to get the Application object.
	 *
	 * Via this method any layer can access the Application object and subsequently any other layer.
	 *
	 * @return The Application object reference.
	 */
	Application &app() { return *m_app.lock(); }

	/// Method to set the Application object.
	void setApp(const std::shared_ptr<Application> &app) { m_app = app; }

protected:
	/// ImGui flags used for the window. These cna be override in the child classes.
	ImGuiWindowFlags m_windowFlags;

	// Name of the layer is used for identification in the LayerStack.
	std::string m_name;

	// This needs to be a weak_ptr to avoid circular references
	// and to ensure that the Application object is destroyed
	// and subsequently all its layers
	std::weak_ptr<Application> m_app;
};
