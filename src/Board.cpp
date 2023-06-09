#include "Board.h"
#include "Icon.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <iostream>
#include <sstream>
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

	ImVec4 green = (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f, 0.5f);

	m_tiles.clear();
	m_tiles.resize(m_height);
	for (int y = 0; y < m_height; y++) {
		std::vector<Tile> row;
		for (size_t x = 0; x < m_width; x++) {
			int cnt = countSurroundingMines(x, y);
			row.emplace_back(Tile(m_icons[0], green));
		}
		m_tiles[y] = row;
	}
}

void SelectableColor(ImU32 color)
{
	ImVec2 p_min = ImGui::GetItemRectMin();
	ImVec2 p_max = ImGui::GetItemRectMax();
	ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
}

void Board::render()
{
	if (m_gameOver) {
		ImGui::Begin("Game Over", NULL, m_windowFlags);
		ImGui::Text("Game Over");
		if (ImGui::Button("Play Again")) {
			m_gameOver = false;
			m_initialized = false;
			for (auto &row : m_tiles) {
				for (auto &tile : row) {
					tile.click(false);
				}
			}

		}
		ImGui::End();
	}

	int buttonWidth = (ImGui::GetWindowSize().x) / m_width - 1;
	int buttonHeight = (ImGui::GetWindowSize().y) / m_height - 1;

	ImGui::Begin("Board", NULL, m_windowFlags);

	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			if (x > 0) {
				ImGui::SameLine(x*(buttonWidth + 4) + 8);
			}
			int id = y * m_tiles.front().size() + x;
			ImGui::PushID(id);

			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)m_tiles[y][x].color());
			if (m_tiles[y][x].clicked()) {
				setButtonColor(x, y);
				int h = m_tiles[y][x].ocupant()->textureHeight();
				int w = m_tiles[y][x].ocupant()->textureWidth();
				if (ImGui::ImageButton((void *)(intptr_t)m_tiles[y][x].ocupant()->texture(), ImVec2(buttonWidth - 8, buttonHeight - 6))) {
					if (m_tiles[y][x].belongsToUs(m_icons[(int)Icon::Ocupant::Flag])) {
						unmarkMine(x, y);
					}
				}
				ImGui::PopStyleColor(2);
			}
			else {
				setButtonColor(x, y);
				if (ImGui::Button("", ImVec2(buttonWidth, buttonHeight), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight )) {
					if (isTilePlayable(x, y)) {
						if (!m_initialized) {
							initTiles(x, y);
						}

						if (ImGui::IsMouseDown(ImGuiMouseButton_Right)
							|| ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
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
	m_gameOver = false;
	m_initialized = false;
	for (auto &row : m_tiles) {
		for (auto &tile : row) {
			tile.click(false);
		}
	}
	return *this;
}

void Board::initTiles(int x, int y)
{
	m_minePositions = generateMinePositions(x, y);
	ImVec4 green = (ImVec4)ImColor::HSV(0.3f, 0.6f, 0.6f, 0.5f);

	m_tiles.clear();
	m_tiles.resize(m_height);
	for (int y = 0; y < m_height; y++) {
		std::vector<Tile> row;
		for (size_t x = 0; x < m_width; x++) {
			int cnt = countSurroundingMines(x, y);
			Icon::Ptr icon = m_icons[cnt];

			row.emplace_back(Tile(icon, green));
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

void Board::setButtonColor(int x, int y)
{
	if (isTilePlayable(x, y)) {
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(7.0f, 0.8f, 0.8f));
	}
	else {
		
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)m_tiles[y][x].color());
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)m_tiles[y][x].color());
	}
}

bool Board::isTilePlayable(int x, int y)
{
	return !m_tiles[y][x].clicked();
}

bool Board::tileExists(int x, int y)
{
	return x >= 0 && x < m_numberOfMines && y >= 0 && y < m_numberOfMines;
}

bool Board::isGamePlayable()
{
	for (size_t i = 0; i < m_numberOfMines; i++) {
		for (size_t j = 0; j < m_numberOfMines; j++) {
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

void Board::setAllTilesClicked()
{
	for(auto &line : m_tiles) {
		for(auto &tile : line) {
			tile.click();
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
			m_tiles[y][x].setOcupant(m_icons[(int)Icon::Ocupant::Mine]);
			return;
		}
	}

	int cnt = countSurroundingMines(x, y);
	m_tiles[y][x].setOcupant(m_icons[cnt]).click(false);
}
