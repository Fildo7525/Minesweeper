#pragma once

#include <vector>

#include "Icon.h"

class Icons
{
	Icons();
	~Icons();
	void loadIcons();

public:
	static Icons &instance();

	Icon::Ptr icon(Icon::Ocupant ocupant);

private:
	std::vector<Icon::Ptr> m_icons;
};

