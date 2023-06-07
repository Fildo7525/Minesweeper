#pragma once

#include "Application.h"
#include "Layer.h"
#include "Player.h"
#include "Tile.h"

#include <memory>
#include <vector>

class Board
	: public Layer
{
public:
	using Tiles = std::vector<std::vector<Tile>>;
	explicit Board(int boardSize);
	void render() override;

	Board &setBoardSize(int size);
	int &boardSize() { return m_boardSize; }
	std::vector<int> scores() const;
	std::vector<std::shared_ptr<Player>> players() const;
	std::shared_ptr<Player> currentPlayer() const;
	bool isGameOver() const { return m_gameOver; }

private:
	struct Direction {
		int x;
		int y;
	};

	void initTiles();

	/**
	 * @brief Detects wether the coordinates are in the middle of the board and who is the owner.
	 *
	 * @param y Y coordiante.
	 * @param x X coordinate.
	 * @return Ocupant of the tile.
	 */
	Player::Ocupant middleTilePlayer(int y, int x);
	int changeOponentTiles(int x, int y);
	void setButtonColor(int x, int y);
	int isTilePlayable(int x, int y);

	int checkDirection(int x, int y, const Direction &init, const Direction &dir);
	bool tileExists(int x, int y);

	void findBestTileCapture();
	bool isGamePlayable();
	int getPlayerScore(std::shared_ptr<Player> player) const;

private:
	std::shared_ptr<Player> m_currentPlayer;
	std::vector<std::shared_ptr<Player>> m_players;
	Tiles m_tiles;
	bool m_gameOver;
	int m_boardSize;
};
