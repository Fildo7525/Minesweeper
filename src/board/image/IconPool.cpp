#include "IconPool.h"

std::vector<Icon::Ptr> m_icons;

void loadIcons()
{
	m_icons = {
		std::make_shared<Icon>(Icon::Ocupant::Empty, "", 0, 0),
		std::make_shared<Icon>(Icon::Ocupant::One, "../images/one.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Two, "../images/two.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Three, "../images/three.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Four, "../images/four.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Five, "../images/five.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Six, "../images/six.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Seven, "../images/seven.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Eight, "../images/eight.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Mine, "../images/mine_icon.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::Flag, "../images/mine_flag.png", 10, 10),
		std::make_shared<Icon>(Icon::Ocupant::WrongFlag, "../images/mine_wrong_flag.png", 10, 10),
	};
}
