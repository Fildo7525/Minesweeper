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

	/**
	 * @brief Check if the tile belongs to the given player.
	 *
	 * @see Icon::Ocupant The enum representing the players.
	 *
	 * @param ocupant The ocupant to check the ownership.
	 * @return True if the tile belongs to the given player, false otherwise.
	 */
	bool belongsTo(const Icon::Ocupant &ocupant) const;

	/**
	 * @brief Setup the tile with a given ocupant.
	 *
	 * @param ocupant The ocupant to set the tile to.
	 * @return The reference to the tile object.
	 */
	Tile &setOcupant(const Icon::Ocupant &ocupant) { m_ocupant = ocupant; return *this; }

	/**
	 * Returns the position of the tile.
	 */
	Pose position() const { return m_position; }

	/**
	 * True if the tile was clicked, false otherwise.
	 */
	bool clicked() const { return m_clicked; }

	/**
	 * @brief Click/Unclick the tile.
	 *
	 * @param c True if the tile was clicked, false means the tile was unclicked.
	 * @return The reference to the tile object.
	 */
	Tile &click(bool c = true);

	/**
	 * @brief Render the tile.
	 *
	 * The color of the tile is determined by clicked state and the ocupant.
	 *
	 * @return ImColor representation of the tile in the current state.
	 */
	ImColor color() const;

	/**
	 * Get the ocupant of the tile.
	 */
	Icon::Ocupant ocupant() const;

	/**
	 * Get the icon of the tile.
	 *
	 * @see Icon::Ocupant The enum representation of the tile state.
	 */
	std::shared_ptr<Icon> icon() const { return m_icons[(int)m_ocupant]; }

private:

	ImColor m_color;
	Icon::Ocupant m_ocupant;
	bool m_clicked;
	Pose m_position;
};
