#pragma once

#include "IconPool.h"

#include "imgui.h"
#include "Icon.h"

#include <memory>
#include <vector>

struct Pose {
	int x;
	int y;

};

bool operator==(const Pose &lhs, const Pose &rhs);

namespace std
{
	template <>
	struct hash<Pose>
	{
		size_t operator()(Pose const &pose) const noexcept
		{
			return std::hash<int>{}(pose.x) ^ (std::hash<int>{}(pose.y) << 1);
		}
	};
}

class Tile
{
public:
	explicit Tile(Icon::Ocupant ocupant, Pose position, bool clicked = false);

	bool belongsTo(const Icon::Ocupant &ocupant) const;

	Tile &setOcupant(const Icon::Ocupant &ocupant) { m_ocupant = ocupant; return *this; }

	Pose position() const { return m_position; }
	bool clicked() const { return m_clicked; }
	Tile &click(bool c = true); //{}

	ImColor color() const;
	bool selected() const { return m_ocupant != Icon::Ocupant::Empty; }
	Icon::Ocupant ocupant() const;
	std::shared_ptr<Icon> icon() const { return m_icons[(int)m_ocupant]; }

private:

	ImColor m_color;
	Icon::Ocupant m_ocupant;
	bool m_clicked;
	Pose m_position;
};
