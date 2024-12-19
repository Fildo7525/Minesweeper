#include "Board.h"

#include "IconPool.h"
#include "imgui.h"

#include <algorithm>
#include <execution>
#include <random>

bool operator==(const Pose &lhs, const Pose &rhs)
{
	return rhs.x == lhs.x && rhs.y == lhs.y;
}

Board::Board(int width, int height, int numberOfMines)
	: Layer("Board")
	, m_initialized(false)
	, m_minePositions(numberOfMines)
	, m_gameOver(GameOverState::Playing)
	, m_width(width)
	, m_height(height)
	, m_numberOfMines(numberOfMines)
	, m_numberOfFlags(0)
	, m_start(nullptr)
	, m_difficulty(0)
{
	Icons::instance();
	setupEmptyTiles();
}

void SelectableColor(ImU32 color)
{
	ImVec2 p_min = ImGui::GetItemRectMin();
	ImVec2 p_max = ImGui::GetItemRectMax();
	ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
}

void Board::render()
{
	if (m_gameOver > GameOverState::Playing || !isGamePlayable()) {
		std::for_each(std::execution::par_unseq, m_tiles.begin(), m_tiles.end(), [&](auto &row) {
			for (auto &tile : row) {
				if (tile.belongsTo(Icon::Ocupant::Flag) && m_minePositions.find(tile.position()) == m_minePositions.end()) {
					tile.setOcupant(Icon::Ocupant::WrongFlag);
				}
				tile.click();
			}
		});
	}

	if (not ImGui::Begin("Board", NULL, m_windowFlags)) {
		throw std::runtime_error("Could not create board window");
	}

	auto size = ImGui::GetWindowSize();
	int buttonWidth = size.x / (m_width + 1);
	int buttonHeight = size.y / (m_height + 1);
	int buttonSize = buttonWidth < buttonHeight ? buttonWidth : buttonHeight;

	auto buttonFlags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight;

	ImGuiStyle &style = ImGui::GetStyle();
	style.ItemSpacing = ImVec2(1, 1);
	style.FrameRounding = 2.0f;

	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			if (x > 0) {
				ImGui::SameLine();
			}
			int id = y * m_tiles.front().size() + x;
			ImGui::PushID(id);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)m_tiles[y][x].color());
			if (m_tiles[y][x].clicked()) {
				handleClickedTile(buttonSize, x, y, buttonFlags);
			}
			else {
				handleUnclickedTile(buttonSize, x, y, buttonFlags);
			}
			ImGui::PopStyleColor(1);
			ImGui::PopID();
		}
	}
	ImGui::End();
}

Board &Board::setNumberOfMines(int size)
{
	m_numberOfMines = size;
	m_numberOfFlags = 0;
	m_gameOver = GameOverState::Playing;
	m_initialized = false;

	for (auto &row : m_tiles) {
		for (auto &tile : row) {
			tile.click(false);
		}
	}
	return *this;
}

long Board::elapsedTime()
{
	if (m_start == nullptr) {
		return -1;
	}

	if (m_gameOver == GameOverState::Playing) {
		auto diff = std::chrono::steady_clock::now() - *m_start;
		m_lastElapsedTime = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
	}

	return m_lastElapsedTime;
}

void Board::setDifficulty(int difficulty, bool reconfigure)
{
	m_difficulty = difficulty;
	if (!reconfigure) {
		return;
	}

	m_height = sizeFromDifficulty();
	m_width = m_height;

	setupEmptyTiles();
	setNumberOfMines((m_height*m_width) / 5);
	resetTimer();
}

void Board::setupEmptyTiles()
{
	m_tiles.clear();
	m_tiles.resize(m_height);
	for (int y = 0; y < m_height; y++) {
		std::vector<Tile> row;
		for (int x = 0; x < m_width; x++) {
			row.emplace_back(Tile(Icon::Ocupant::Empty, {x,y}));
		}
		m_tiles[y] = row;
	}
}

void Board::on_refreshBoard_activated()
{
	m_gameOver = GameOverState::Playing;
	m_initialized = false;
	m_numberOfClicks = 0;
	m_numberOfFlags = 0;

	resetTimer();

	std::for_each(std::execution::par_unseq, m_tiles.begin(),  m_tiles.end(), [](auto &row) {
		for (auto &tile : row) {
			tile.click(false);
		}
	});
}

void Board::initTiles(int X, int Y)
{
	m_minePositions = generateMinePositions(X, Y);
	m_numberOfClicks = 0;

	m_tiles.clear();
	m_tiles.resize(m_height);

	for (int y = 0; y < m_height; y++) {
		std::vector<Tile> row;
		row.reserve(m_width);

		for (int x = 0; x < m_width; x++) {
			auto ocupant = (Icon::Ocupant)countSurroundingMines(x, y);
			row.emplace_back(Tile(ocupant, {x, y}));
		}

		m_tiles[y] = row;
	}

	m_initialized = true;
}

bool Board::isInRange(Pose pose, Pose generated, int range)
{
	return (generated.x >= pose.x - range && generated.x <= pose.x + range) &&
		(generated.y >= pose.y - range && generated.y <= pose.y + range);
}

std::unordered_set<Pose> Board::generateMinePositions(int x, int y)
{
	std::unordered_set<Pose> minePositions;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution hDis(0, m_height - 1);
	std::uniform_int_distribution wDis(0, m_width - 1);

	while (minePositions.size() < m_numberOfMines) {
		int X = wDis(gen);
		int Y = hDis(gen);

		// Ensures that the mines will not be generated around the first click.
		if (isInRange({x, y}, {X, Y}, 1)) {
			continue;
		}

		minePositions.insert({X, Y});
	}
	return minePositions;
}

int Board::countSurroundingMines(int x, int y)
{
	int count = 0;
	if (m_minePositions.find({x, y}) != m_minePositions.end()) {
		return 9;
	}

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (!tileExists(x,y)) {
				continue;
			}
			if (m_minePositions.find({x + j, y + i}) != m_minePositions.end()) {
				count++;
			}
		}
	}
	return count;
}

int Board::countSurroundingFlags(int x, int y)
{
	int count = 0;
	if (m_tiles[y][x].belongsTo(Icon::Ocupant::Flag)) {
		return 9;
	}

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (!tileExists(x + j, y + i)) {
				continue;
			}
			if (m_tiles[y + i][x + j].belongsTo(Icon::Ocupant::Flag)) {
				count++;
			}
		}
	}

	return count;
}

void Board::setButtonColor(int x, int y)
{
	if (isTilePlayable(x, y)) {
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.3f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)m_tiles[y][x].color());
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)m_tiles[y][x].color());
	}
}

bool Board::isTilePlayable(int x, int y)
{
	const auto &tile = m_tiles[y][x];
	return !tile.clicked();
}

bool Board::tileExists(int x, int y)
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

bool Board::isGamePlayable()
{
	if (m_numberOfFlags != m_numberOfMines) {
		return true;
	}

	for (size_t i = 0; i < m_width; i++) {
		for (size_t j = 0; j < m_height; j++) {
			if (isTilePlayable(j, i)) {
				return true;
			}
		}
	}
	return false;
}

void Board::clickAllEmptyTiles(int x, int y)
{
	if (!tileExists(x, y)) {
		return;
	}
	if (m_tiles[y][x].clicked()) {
		return;
	}
	m_tiles[y][x].click();
	if (countSurroundingMines(x, y) != 0) {
		return;
	}
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; ++j) {
			clickAllEmptyTiles(x + j, y + i);
		}
	}
}

void Board::clickPossibleTiles(int x, int y)
{
	if (!tileExists(x, y)) {
		return;
	}
	if (m_tiles[y][x].clicked()) {
		return;
	}

	m_tiles[y][x].click();

	if (m_tiles[y][x].ocupant() == Icon::Ocupant::Mine) {
		m_gameOver = GameOverState::Lose;
		return;
	}

	if (countSurroundingFlags(x, y) != (int)m_tiles[y][x].ocupant()) {
		return;
	}
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; ++j) {
			clickPossibleTiles(x + j, y + i);
		}
	}
}

void Board::setAllTilesClicked()
{
	for(int y = 0; y < m_height; y++) {
		for(int x = 0; x < m_height; x++) {
			if (m_minePositions.find({x, y}) == m_minePositions.end()) {
				if (m_tiles[y][x].belongsTo(Icon::Ocupant::Flag)) {
					unmarkMine(x, y);
				}
			}
			m_tiles[y][x].click();
		}
	}
}

void Board::markMine(int x, int y)
{
	m_tiles[y][x].setOcupant(Icon::Ocupant::Flag).click();
}

void Board::unmarkMine(int x, int y)
{
	for(auto &position : m_minePositions) {
		if (position.x == x && position.y == y) {
			m_tiles[y][x].setOcupant(Icon::Ocupant::Mine).click(false);
			return;
		}
	}

	Icon::Ocupant cnt = (Icon::Ocupant)countSurroundingMines(x, y);
	m_tiles[y][x].setOcupant(cnt).click(false);
}

int Board::sizeFromDifficulty()
{
	switch (m_difficulty) {
		case 0:
			return 10;
		case 1:
			return 15;
		case 2:
			return 20;
	}
	return 10;
}


bool Board::allMinesMarked()
{
	for (auto &position : m_minePositions) {
		if (!m_tiles[position.y][position.x].belongsTo(Icon::Ocupant::Flag)) {
			return false;
		}
	}
	return true;
}

void Board::handleUnclickedTile(int buttonSize, int x, int y, int buttonFlags)
{
	setButtonColor(x, y);
	ImVec2 size(buttonSize, buttonSize);

	if (ImGui::Button("", size, buttonFlags )) {
		if (isTilePlayable(x, y)) {
			if (!m_initialized) {
				initTiles(x, y);
				m_start = std::make_shared<time>(std::chrono::steady_clock::now());
			}

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
				m_numberOfFlags++;
				markMine(x, y);
				if (allMinesMarked() && m_numberOfFlags == m_numberOfMines) {
					m_numberOfFlags++;
					m_gameOver = GameOverState::Win;
					setAllTilesClicked();
				}
			}
			else if (m_tiles[y][x].ocupant() == Icon::Ocupant::Mine) {
				m_gameOver = GameOverState::Lose;
				setAllTilesClicked();
			}
			else if (m_tiles[y][x].ocupant() == Icon::Ocupant::Empty) {
				clickAllEmptyTiles(x, y);
			}

			m_tiles[y][x].click();
			m_numberOfClicks++;
		}
	}
	ImGui::PopStyleColor(2);

}

void Board::handleClickedTile(int buttonSize, int x, int y, int buttonFlags)
{
	setButtonColor(x, y);
	ImVec2 size(buttonSize - 8, buttonSize - 6);

	const std::string localID = std::to_string(y * m_tiles.front().size() + x);
	if (ImGui::ImageButton(localID.c_str(), (intptr_t)m_tiles[y][x].icon()->texture(), size, buttonFlags)) {
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			if (m_tiles[y][x].belongsTo(Icon::Ocupant::Flag)) {
				m_numberOfFlags--;
				unmarkMine(x, y);

				if (allMinesMarked() && m_numberOfFlags == m_numberOfMines) {
					m_gameOver = GameOverState::Win;
					setAllTilesClicked();
				}
			}
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			auto ocup = m_tiles[y][x].ocupant();
			if (ocup != Icon::Ocupant::Flag && countSurroundingFlags(x, y) == (int)ocup) {
				m_tiles[y][x].click(false);
				m_numberOfClicks++;
				clickPossibleTiles(x, y);
			}
		}
	}

	ImGui::PopStyleColor(2);
}
