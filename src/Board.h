#pragma once

#include "Application.h"
#include "Layer.h"
#include "Icon.h"
#include "Tile.h"

#include <memory>
#include <vector>
#include <unordered_set>

class Board
	: public Layer
{
public:

	using Tiles = std::vector<std::vector<Tile>>;
	explicit Board(int width, int height, int numberOfMines);
	void render() override;

	Board &setNumberOfMines(int size);
	int &numberOfMines() { return m_numberOfMines; }
	bool isGameOver() const { return m_gameOver; }

private:

	void initTiles(int x, int y);
	bool isInRange(Pose pose, Pose generated, int range);
	std::unordered_set<Pose> generateMinePositions(int x, int y);
	int countSurroundingMines(int x, int y);
	int countSurroundingFlags(int x, int y);
	void setButtonColor(int x, int y);

	bool isTilePlayable(int x, int y);
	bool tileExists(int x, int y);

	void findBestTileCapture();
	bool isGamePlayable();
	void clickAllEmptyTiles(int x, int y);
	void clickPossibleTiles(int x, int y);
	void setAllTilesClicked();

	void markMine(int x, int y);
	void unmarkMine(int x, int y);

private:
	bool m_initialized;
	std::vector<Icon::Ptr> m_icons;
	std::unordered_set<Pose> m_minePositions;
	Tiles m_tiles;
	bool m_gameOver;
	int m_width;
	int m_height;
	int m_numberOfMines;
};
