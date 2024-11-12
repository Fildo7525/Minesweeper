#include "Status.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <string>
#include <sstream>

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define DEFAULT_COLOR ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define SCORE_FILE_NAME "scores.txt"
#define COLUMN_SIZE 2

Status::Status(std::shared_ptr<Board> &board)
	: Layer("Status")
	, m_board(board)
	, m_difficulty(0)
	, m_numberOfMines(board->totalNumberOfMines())
	, m_scoreFile(SCORE_FILE_NAME, std::ios::in)
	, m_scores()
	, m_name("User")
{
	if (!m_scoreFile.is_open()) {
		m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);
	}

	loadScoreFile();
	m_name.resize(32);
}

void Status::render()
{
	if (m_board->isGameOver() == Board::GameOverState::Win) {
		m_score = 100 * (m_board->totalNumberOfCells() * m_numberOfMines) / m_board->numberOfClicks() / m_board->elapsedTime();
		m_scores[m_difficulty].insert({m_score, m_name});
	}

	ImGui::Begin("Game Status", NULL, m_windowFlags);

	ImGui::InputText("Player name", m_name.data(), 32);

	ImGui::Text("Mines: %d / %d", m_board->numberOfFlags(), m_board->totalNumberOfMines());
	auto time = m_board->elapsedTime();
	if (time == -1) {
		ImGui::Text("Elapsed time: 0:0");
	}
	else {
		auto minutes = time / 60;
		ImGui::Text("Elapsed time: %2ld:%ld", minutes, time%60);
	}

	if (ImGui::RadioButton("Easy", &m_difficulty, 0)) {
		m_board->setDifficulty(0);
		m_numberOfMines = m_board->totalNumberOfMines();
	}
	if (ImGui::RadioButton("Medium", &m_difficulty, 1)) {
		m_board->setDifficulty(1);
		m_numberOfMines = m_board->totalNumberOfMines();
	}
	if (ImGui::RadioButton("Hard", &m_difficulty, 2)) {
		m_board->setDifficulty(2);
		m_numberOfMines = m_board->totalNumberOfMines();
	}
	if (ImGui::RadioButton("Custom", &m_difficulty, 3)) {
		m_board->setOnlyDifficulty(3);
		m_localHeight = m_board->height();
		m_localWidth = m_board->width();
	}
	if (m_difficulty == 3) {
		float statusWidth = ImGui::GetWindowWidth();
		ImGui::PushItemWidth(statusWidth / 4.);

		if (ImGui::InputInt("Width", &m_localWidth)) { }

		ImGui::SameLine();
		if (ImGui::InputInt("Height", &m_localHeight)) { }

		ImGui::SameLine();
		if (ImGui::Button("Apply")) {

			// Limit the width and height
			m_board->width() = std::clamp(m_localWidth, 9, 30);
			m_board->height() = std::clamp(m_localHeight, 9, 30);
			m_localHeight = m_board->height();
			m_localWidth = m_board->width();

			m_board->setupEmptyTiles();
			m_numberOfMines = (m_localWidth * m_localHeight) / 5;
			m_board->setNumberOfMines(m_numberOfMines);
			m_board->resetTimer();
		}

		ImGui::PopItemWidth();
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
	if (ImGui::InputInt("Mines", &m_numberOfMines)) {
		m_numberOfMines = std::clamp(m_numberOfMines, 1, m_board->totalNumberOfCells()-9);
		m_board->setNumberOfMines(m_numberOfMines);
		m_board->resetTimer();
	}
	ImGui::PopItemWidth();

	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildR", ImVec2(0, 260), ImGuiChildFlags_Border);

		if (!ImGui::BeginTabBar("MyTabBar"))
			return;

		for (int i = 0; i < m_scores.size(); i++) {
			if (!ImGui::BeginTabItem(difficultyString(i).c_str()))
				continue;

			createTabTable(i);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	ImGui::End();
}

Status::~Status()
{
	m_scoreFile.close();
	m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);

	for (auto &score : m_scores) {
		// Printing difficulty
		m_scoreFile << score.first << '\n';
		for (auto &name : score.second)
			// printing score and name
			m_scoreFile << name.first << " " << name.second << '\n';
	}

	m_scoreFile.flush();
	m_scoreFile.close();
}

std::string Status::difficultyString(int difficulty) const
{
	auto tmp = difficulty == -1 ? m_difficulty : difficulty;
	switch (tmp) {
		case 0:
			return "Easy";
		case 1:
			return "Medium";
		case 2:
			return "Hard";
		case 3:
			return "Custom";
		default:
			return "Unknown";
	}
}

void Status::createTabTable(int difficulty)
{
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	std::string tableID = "ScoreBoard" + std::to_string(difficulty);
	if (!ImGui::BeginTable(tableID.c_str(), COLUMN_SIZE, flags))
		return;

	ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
	ImGui::TableSetupColumn("User name");
	ImGui::TableSetupColumn("Max score");
	ImGui::TableHeadersRow();

	for (auto &diffGrade : m_scores[difficulty]) {
		ImGui::TableNextRow();
		if (m_score == diffGrade.first && difficulty == m_difficulty && m_name == diffGrade.second) {
			ImGui::PushStyleColor(ImGuiCol_Text, RED_COLOR);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, DEFAULT_COLOR);
		}

		for (int column = 0; column < 2; column++) {
			ImGui::TableSetColumnIndex(column);
			ImGui::Text("%s", (column == 1 ? std::to_string(diffGrade.first).c_str() : diffGrade.second.c_str()));
		}

		ImGui::PopStyleColor();
	}
	ImGui::EndTable();
}

std::vector<std::string> Status::split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}

	return elems;
}

void Status::loadScoreFile()
{
	int score;
	std::string line;
	int difficulty = 0;

	while (std::getline(m_scoreFile, line)) {
		auto parts = split(line, ' ');
		if (parts.size() == 1) {
			difficulty = std::stoi(parts[0]);
			m_scores[difficulty] = {};
			continue;
		}

		score = std::stoi(parts[0]);
		line = parts[1];
		m_scores[difficulty].insert({score, line});
	}
}

