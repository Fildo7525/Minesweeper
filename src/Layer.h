#pragma once

#include <iostream>
#include "imgui.h"

class Layer
{
public:
	explicit Layer(const std::string &name)
		: m_windowFlags(0)
		, m_name(name)
	{
		m_windowFlags |= ImGuiWindowFlags_NoTitleBar;
		m_windowFlags |= ImGuiWindowFlags_NoCollapse;
		m_windowFlags |= ImGuiWindowFlags_NoScrollbar;
		m_windowFlags |= ImGuiWindowFlags_NoMove;
		m_windowFlags |= ImGuiWindowFlags_NoResize;
	}

	virtual ~Layer() {}

	virtual void onAttach() {}
	virtual void onDetach() {}

	std::string getName() const { return m_name; }

	virtual void render() {}

protected:
	ImGuiWindowFlags m_windowFlags;
	std::string m_name;
};
