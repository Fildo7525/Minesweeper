#include "Board.h"
#include "Icon.h"
#include "imgui.h"

#include <random>

bool operator==(const Pose &lhs, const Pose &rhs)
{
	return rhs.x == lhs.x && rhs.y == lhs.y;
}

Board::Board(int width, int height, int numberOfMines)
	: Layer("Board")
	, m_initialized(false)
	, m_icons()
	, m_minePositions(numberOfMines)
	, m_gameOver(false)
	, m_width(width)
	, m_height(height)
	, m_numberOfMines(numberOfMines)
	, m_numberOfFlags(0)
	, m_start(nullptr)
	, m_difficulty(0)
{
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Empty, "", 0, 0));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::One, "../images/one.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Two, "../images/two.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Three, "../images/three.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Four, "../images/four.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Five, "../images/five.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Six, "../images/six.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Seven, "../images/seven.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Eight, "../images/eight.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Mine, "../images/mine_icon.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::Flag, "../images/mine_flag.png", 10, 10));
	m_icons.push_back(std::make_shared<Icon>(Icon::Ocupant::WrongFlag, "../images/mine_wrong_flag.png", 10, 10));

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
	if (m_gameOver || !isGamePlayable()) {

		for (auto &row : m_tiles) {
			for (auto &tile : row) {
				if (tile.belongsToUs(m_icons[(int)Icon::Ocupant::Flag]) && m_minePositions.find(tile.position()) == m_minePositions.end()) {
					tile.setOcupant(m_icons[(int)Icon::Ocupant::WrongFlag]);
				}
				tile.click();
			}
		}

		ImGui::Begin("Game Over", NULL, m_windowFlags);
		ImGui::Text("Game Over");
		if (ImGui::Button("Play Again")) {
			m_gameOver = false;
			m_initialized = false;
			m_numberOfFlags = 0;
			resetTimer();

			for (auto &row : m_tiles) {
				for (auto &tile : row) {
					tile.click(false);
				}
			}
		}
		ImGui::End();
	}


	ImGui::Begin("Board", NULL, m_windowFlags);
	int buttonWidth = (ImGui::GetWindowSize().x - 50) / m_width - 1;
	int buttonHeight = (ImGui::GetWindowSize().y - 80) / m_height - 1;
	int buttonSize = buttonWidth < buttonHeight ? buttonWidth : buttonHeight;

	auto buttonFlags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight;

	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			if (x > 0) {
				ImGui::SameLine(x*(buttonSize + 4) + 8);
			}
			int id = y * m_tiles.front().size() + x;
			ImGui::PushID(id);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)m_tiles[y][x].color());
			if (m_tiles[y][x].clicked()) {
				setButtonColor(x, y);
				ImVec2 size(buttonSize - 8, buttonSize - 6);

				if (ImGui::ImageButton((void *)(intptr_t)m_tiles[y][x].ocupant()->texture(), size, buttonFlags)) {
					if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
						if (m_tiles[y][x].belongsToUs(m_icons[(int)Icon::Ocupant::Flag])) {
							m_numberOfFlags--;
							unmarkMine(x, y);
						}
					}
					else {
						auto ocup = m_tiles[y][x].ocupant()->ocupation();
						if (ocup != Icon::Ocupant::Flag && countSurroundingFlags(x, y) == (int)ocup) {
							m_tiles[y][x].click(false);
							clickPossibleTiles(x, y);
						}
					}
				}

				ImGui::PopStyleColor(2);
			}
			else {
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
						}

						else if (m_tiles[y][x].ocupant()->ocupation() == Icon::Ocupant::Mine) {
							m_gameOver = true;
							setAllTilesClicked();
						}
						else if (m_tiles[y][x].ocupant()->ocupation() == Icon::Ocupant::Empty) {
							clickAllEmptyTiles(x, y);
						}
						m_tiles[y][x].click();
					}
				}
				ImGui::PopStyleColor(2);
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
	m_gameOver = false;
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

	if (!m_gameOver) {
		auto diff = std::chrono::steady_clock::now() - *m_start;
		m_lastElapsedTime = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
	}

	return m_lastElapsedTime;
}

void Board::setDifficulty(int difficulty)
{
	m_difficulty = difficulty;
	m_height = getSizeFromDifficulty();
	m_width = m_height;
	setupEmptyTiles();
	setNumberOfMines((m_height*m_width) / 5);
	resetTimer();
}

void Board::setupEmptyTiles()
{
	ImVec4 green = (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f, 0.5f);

	m_tiles.clear();
	m_tiles.resize(m_height);
	for (int y = 0; y < m_height; y++) {
		std::vector<Tile> row;
		for (int x = 0; x < m_width; x++) {
			int cnt = countSurroundingMines(x, y);
			row.emplace_back(Tile(m_icons[0], green, {x,y}));
		}
		m_tiles[y] = row;
	}
}

void Board::initTiles(int X, int Y)
{
	m_minePositions = generateMinePositions(X, Y);
	ImVec4 green = (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f, 0.5f);

	m_tiles.clear();
	m_tiles.resize(m_height);
	for (int y = 0; y < m_height; y++) {
		std::vector<Tile> row;
		for (int x = 0; x < m_width; x++) {
			int cnt = countSurroundingMines(x, y);
			Icon::Ptr icon = m_icons[cnt];

			row.emplace_back(Tile(icon, green, {x, y}));
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
	if (m_tiles[y][x].belongsToUs(m_icons[(int)Icon::Ocupant::Flag])) {
		return 9;
	}

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (!tileExists(x + j, y + i)) {
				continue;
			}
			if (m_tiles[y + i][x + j].belongsToUs(m_icons[(int)Icon::Ocupant::Flag])) {
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

	if (m_tiles[y][x].ocupant()->ocupation() == Icon::Ocupant::Mine) {
		m_gameOver = true;
		return;
	}

	if (countSurroundingFlags(x, y) != (int)m_tiles[y][x].ocupant()->ocupation()) {
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
				if (m_tiles[y][x].belongsToUs(m_icons[(int)Icon::Ocupant::Flag])) {
					unmarkMine(x, y);
				}
			}
			m_tiles[y][x].click();
		}
	}
}

void Board::markMine(int x, int y)
{
	m_tiles[y][x].setOcupant(m_icons[(int)Icon::Ocupant::Flag]).click();
}

void Board::unmarkMine(int x, int y)
{
	for(auto &position : m_minePositions) {
		if (position.x == x && position.y == y) {
			m_tiles[y][x].setOcupant(m_icons[(int)Icon::Ocupant::Mine]).click(false);
			return;
		}
	}

	int cnt = countSurroundingMines(x, y);
	m_tiles[y][x].setOcupant(m_icons[cnt]).click(false);
}

int Board::getSizeFromDifficulty()
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

