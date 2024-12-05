#pragma once

#include "Layer.h"
#include "Tile.h"

#include <chrono>
#include <unordered_set>
#include <vector>

class Board
	: public Layer
{
public:
	enum class GameOverState
	{
		Playing,
		Win,
		Lose,
		Waiting,
	};

	using time = std::chrono::time_point<std::chrono::steady_clock>;

	using Tiles = std::vector<std::vector<Tile>>;
	explicit Board(int width, int height, int numberOfMines);
	void render() override;

	Board &setNumberOfMines(int size);
	int totalNumberOfMines() const { return m_numberOfMines; }
	int numberOfFlags() const { return m_numberOfFlags; }
	GameOverState isGameOver() const { return m_gameOver; }
	long elapsedTime();
	void resetTimer() { m_start = nullptr; }
	const int &difficulty() const { return m_difficulty; }
	const long &numberOfClicks() const { return m_numberOfClicks; }
	void setDifficulty(int difficulty, bool reconfigure = true);
	int totalNumberOfTiles() const { return m_width * m_height; }
	int &width() { return m_width; }
	int &height() { return m_height; }
	void setupEmptyTiles();

	void on_refreshBoard_activated();
	void ackGameOver() { m_gameOver = GameOverState::Waiting; }

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

	int sizeFromDifficulty();
	bool allMinesMarked();

	void handleUnclickedTile(int buttonSize, int x, int y, int buttonFlags);
	void handleClickedTile(int buttonSize, int x, int y, int buttonFlags);

private:
	bool m_initialized;
	std::unordered_set<Pose> m_minePositions;
	Tiles m_tiles;
	GameOverState m_gameOver;
	int m_width;
	int m_height;
	int m_numberOfMines;
	int m_numberOfFlags;
	std::shared_ptr<time> m_start;
	int m_difficulty;
	long m_lastElapsedTime;
	long m_numberOfClicks;
};

