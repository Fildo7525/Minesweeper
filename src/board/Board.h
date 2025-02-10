#pragma once

#include "Layer.h"
#include "Tile.h"

#include <chrono>
#include <unordered_set>
#include <vector>

/**
 * @class Board
 * @brief The Board class is a Layer that represents the game board.
 *
 * This class encapsulates all the tiles and keeps their ownership. At the same time it keeps the overall state
 * of the game. It is responsible for rendering the board and handling the user input.
 *
 * @see Layer Base class for all the layers.
 * @see Tile Class representing the state of the separate tiles.
 */
class Board
	: public Layer
{
public:
	/// The state of the game.
	enum class GameOverState
	{
		Playing,
		Win,
		Lose,
		Waiting,
	};

	static std::shared_ptr<Board> create(int width, int height, int numberOfMines)
	{
		return std::make_shared<Board>(width, height, numberOfMines);
	}

	/// The type of the time point. Used in tracking the user time taken to solve the puzzle.
	using time = std::chrono::time_point<std::chrono::steady_clock>;
	/// The type of the pose. Used in tracking the position of the tiles.
	using Tiles = std::vector<std::vector<Tile>>;

	/// \addgroup Layer
	/// @{
	void render() override;
	/// @}

	/**
	 * @brief Set the number of mines on the board.
	 *
	 * @param size The number of mines to be placed on the board.
	 * @return The reference to the board.
	 */
	Board &setNumberOfMines(int size);

	/// Get the total number of mines on the board.
	int totalNumberOfMines() const { return m_numberOfMines; }

	/// Get the number of flags placed on the board.
	int numberOfFlags() const { return m_numberOfFlags; }

	/// Get the number of mines left to be marked.
	GameOverState isGameOver() const { return m_gameOver; }

	/// Get the number of elapsed time since the game started.
	long elapsedTime();

	/// Reset the timer to the initial state.
	void resetTimer() { m_start = nullptr; }

	/// Get the difficulty of the game.
	const int &difficulty() const { return m_difficulty; }

	/// Get the number of clicks made by the user.
	const long &numberOfClicks() const { return m_numberOfClicks; }

	/**
	 * @brief Set the difficulty of the game.
	 *
	 * @param difficulty The difficulty of the game.
	 * @param reconfigure If the board should reconfigure itself to the new difficulty.
	 */
	void setDifficulty(int difficulty, bool reconfigure = true);

	/// Get the total number of tiles on the board.
	int totalNumberOfTiles() const { return m_width * m_height; }

	/// Get the width of the board.
	int &width() { return m_width; }

	// Get the height of the board.
	int &height() { return m_height; }

	/**
	 * @brief Clear the board and resize it to the new dimensions.
	 *
	 * If the board is already initialized, the tiles inside are destoryed and reinitialized.
	 */
	void setupEmptyTiles();

	/**
	 * @brief Acknowledge the game over state.
	 *
	 * The statistics are saved in a priority queue with dynamic sorting.
	 * This acknowledge ensures that after the game over invocation the result will not be written multiple times to the
	 * score structure.
	 */
	void ackGameOver() { m_gameOver = GameOverState::Waiting; }

	/**
	 * @brief Callback method called when the user wants to refresh the board.
	 *
	 * The difference with @c setupEmptyTiles is that this method resets the timer
	 * but the board dimensions and the number of mines are preserved.
	 */
	void on_refreshBoard_activated();

private:

	/**
	 * @brief Constructor for the Board class.
	 *
	 * The constructor initializes the board with the given width, height and number of mines.
	 * At the construction all of the textures are loaded to be used throughout the game. The board alocates space for all
	 * the time and initializes the tiles with no owner.
	 *
	 * @param width The number of tiles in the horizontal direction.
	 * @param height The number of tiles in the vertical direction.
	 * @param numberOfMines Number of mines to be placed on the board.
	 *
	 * @see Icon Class representing the textures used in the game.
	 */
	explicit Board(int width, int height, int numberOfMines);

	/**
	 * @brief Initialize the tiles on the board.
	 *
	 * Before this method is called all the tiles are empty. This method generates the mines.
	 * The mines are not generated on the button clicked or in its vicinity. This ensures that
	 * the first click always openes a field of empty tiles.
	 *
	 * @param x X coordinate of the clicked button.
	 * @param y Y coordinate of the clicked button.
	 */
	void initTiles(int x, int y);

	/**
	 * @brief Checkes if the @c generated position is in the range of the given position.
	 *
	 * @param pose Ground truth position which is used as a reference.
	 * @param generated Position to be checked.
	 * @param range Number of tiles in the vicinity of the reference position to be checked.
	 * @return True if the generated position is in the range of the reference position, false otherwise.
	 */
	bool isInRange(Pose pose, Pose generated, int range);

	/**
	 * @brief Generates the positions of the mines on the board.
	 *
	 * The number of generated mines is limited by @c m_numberOfMines. The mines are placed randomly on the board.
	 * The position of the mines is only restrected by the position of the first clicked button.
	 *
	 * @param x X coordinate of the clicked button.
	 * @param y Y coordinate of the clicked button.
	 *
	 * @see initTiles Method that initializes the tiles on the board.
	 *
	 * @return Set of the positions of the mines.
	 */
	std::unordered_set<Pose> generateMinePositions(int x, int y);

	/**
	 * @brief Count how many mines are in the vicinity of the given position.
	 *
	 * The maximum number is 8 tiles. The method counts the number of mines in the vicinity of the given position.
	 *
	 * @param x X coordinate of the position.
	 * @param y Y coordinate of the position.
	 * @return Number of mines in the vicinity of the given position.
	 */
	int countSurroundingMines(int x, int y);

	/**
	 * @brief Just like @c countSurroundingMines but counts the number of flags.
	 */
	int countSurroundingFlags(int x, int y);

	/**
	 * @brief Sets the color of the button on the given position when hovered and when clicked.
	 */
	void setButtonColor(int x, int y);

	/**
	 * Check if the tile on the given position is playable.
	 */
	bool isTilePlayable(int x, int y);

	/**
	 * Check if the tile on the given position is in bounds of the board.
	 */
	bool tileExists(int x, int y);

	/**
	 * @brief Check if at least on tile is playable on the board.
	 *
	 * @see isTilePlayable Method that checks if the tile is playable.
	 *
	 * @return True if at least one tile is playable, false otherwise.
	 */
	bool isGamePlayable();

	/**
	 * @brief Recursive function to open the fields that do not have any mines in the vicinity.
	 *
	 * @param x X coordinate of the start position.
	 * @param y Y coordinate of the start position.
	 */
	void clickAllEmptyTiles(int x, int y);

	/**
	 * @brief Recursive function to open the fields that do not have any mines in the vicinity.
	 *
	 * If the starting position does have a mine on it, the @c m_gameOver is set to @c GameOverState::Lose.
	 *
	 * @param x X coordinate of the start position.
	 * @param y Y coordinate of the start position.
	 */
	void clickPossibleTiles(int x, int y);

	/**
	 * @brief Click all the tiles on the board.
	 *
	 * The method is used when the game is over. It opens all the tiles and marks the mines.
	 */
	void setAllTilesClicked();

	/// Right click on the tile.
	void markMine(int x, int y);

	/// Right click on the tile.
	void unmarkMine(int x, int y);

	/// Get the size of the board based on the difficulty.
	int sizeFromDifficulty();

	/// Check if all the mines are marked correctly on the board.
	bool allMinesMarked();

	/// Used in rendering the unclicked tiles.
	void handleUnclickedTile(int buttonSize, int x, int y, int buttonFlags);

	/// Used in rendering the clicked tiles.
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

