#pragma once

#include "Icon.h"
#include "Layer.h"
#include "Tile.h"

#include <chrono>
#include <unordered_set>
#include <vector>

class Board
	: public Layer
{
public:
	using time = std::chrono::time_point<std::chrono::steady_clock>;

	using Tiles = std::vector<std::vector<Tile>>;
	explicit Board(int width, int height, int numberOfMines);
	void render() override;

	Board &setNumberOfMines(int size);
	int totalNumberOfMines() const { return m_numberOfMines; }
	int numberOfFlags() const { return m_numberOfFlags; }
	bool isGameOver() const { return m_gameOver; }
	long elapsedTime();
	void resetTimer() { m_start = nullptr; }
	int &getDifficulty() { return m_difficulty; }
	void setDifficulty(int difficulty);
	int totalNumberOfCells() const { return m_width * m_height; }

private:

	void setupEmptyTiles();
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

	int getSizeFromDifficulty();
	bool allMinesMarked();

private:
	bool m_initialized;
	std::vector<Icon::Ptr> m_icons;
	std::unordered_set<Pose> m_minePositions;
	Tiles m_tiles;
	bool m_gameOver;
	int m_width;
	int m_height;
	int m_numberOfMines;
	int m_numberOfFlags;
	std::shared_ptr<time> m_start;
	int m_difficulty;
	long m_lastElapsedTime;
};

