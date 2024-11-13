#include "Status.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <string>
#include <sstream>

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define DEFAULT_COLOR ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define SCORE_FILE_NAME "scores.txt"
#define INDENT_CUSTOM_SIZE 25
#define MAX_SIZE 30
#define MIN_SIZE 9
#define CUSTOM_DIFFICULTY 3
#define COLUMN_SIZE(dif) (dif == CUSTOM_DIFFICULTY ? 4 : 2)

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
		m_score.score = 100 * (m_board->totalNumberOfCells() * m_numberOfMines) / m_board->numberOfClicks() / m_board->elapsedTime();
		m_score.name = m_name;
		m_score.difficulty = m_difficulty;
		m_score.width = m_localWidth;
		m_score.height = m_localHeight;
		m_score.numberOfMines = m_numberOfMines;

		m_scores[m_difficulty].insert({m_score.score, m_score});
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

	for (int i = 0; i <= CUSTOM_DIFFICULTY; i++) {
		if (not ImGui::RadioButton(difficultyString(i).c_str(), &m_difficulty, i)) {
			continue;
		}

		m_numberOfMines = m_board->totalNumberOfMines();

		if (m_difficulty != CUSTOM_DIFFICULTY) {
			m_board->setDifficulty(i);
		}
		else {
			m_board->setOnlyDifficulty(i);
			m_localHeight = m_board->height();
			m_localWidth = m_board->width();
		}
	}
	if (m_difficulty == 3) {
		float statusWidth = ImGui::GetWindowWidth();
		ImGui::PushItemWidth(statusWidth / 4.);

		ImGui::Indent(INDENT_CUSTOM_SIZE);
		if (ImGui::InputInt("Width", &m_localWidth))
			m_localWidth = std::clamp(m_localWidth, MIN_SIZE, MAX_SIZE);

		/* ImGui::SameLine(); */
		if (ImGui::InputInt("Height", &m_localHeight))
			m_localHeight = std::clamp(m_localHeight, MIN_SIZE, MAX_SIZE);

		/* ImGui::SameLine(); */
		if (ImGui::Button("Apply")) {

			// Limit the width and height
			m_board->width() = m_localWidth;
			m_board->height() = m_localHeight;

			m_board->setupEmptyTiles();
			m_numberOfMines = (m_localWidth * m_localHeight) / 5;
			m_board->setNumberOfMines(m_numberOfMines);
			m_board->resetTimer();
		}
		ImGui::Unindent(INDENT_CUSTOM_SIZE);

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

	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.01f, 1, 1));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.01f, 0.64f,0.78f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.01f, 1, 0.5f));

	if (ImGui::Button("Play again", ImVec2(ImGui::GetWindowWidth(), 0))) {
		m_board->on_refreshBoard_activated();
	}

	ImGui::PopStyleColor(3);

	ImGui::End();
}

Status::~Status()
{
	m_scoreFile.close();
	m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);

	for (auto &score : m_scores) {
		// Printing difficulty
		m_scoreFile << score.first << '\n';
		for (auto &record : score.second) {
			// printing score and name
			m_scoreFile << record.first << " " << record.second.name;

			if (score.first == CUSTOM_DIFFICULTY) {
				// printing width and height
				m_scoreFile << " " << record.second.width << " " << record.second.height;
				// printing number of mines
				m_scoreFile << " " << record.second.numberOfMines;
			}

			m_scoreFile << '\n';
		}
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
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_BordersOuter
		| ImGuiTableFlags_BordersV
		| ImGuiTableFlags_Hideable
		| ImGuiTableFlags_Reorderable
		| ImGuiTableFlags_Resizable
		| ImGuiTableFlags_RowBg;

	std::string tableID = "ScoreBoard" + std::to_string(difficulty);
	if (!ImGui::BeginTable(tableID.c_str(), COLUMN_SIZE(difficulty), flags))
		return;

	ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
	ImGui::TableSetupColumn("User name");
	ImGui::TableSetupColumn("Score");

	if (difficulty == CUSTOM_DIFFICULTY) {
		ImGui::TableSetupColumn("Size");
		ImGui::TableSetupColumn("Mines");
	}

	ImGui::TableHeadersRow();

	for (auto &diffGrade : m_scores[difficulty]) {
		ImGui::TableNextRow();

		// TODO: Add operator== to ScoreRecord
		if (m_score.score == diffGrade.first && difficulty == m_score.difficulty && m_score.name == diffGrade.second.name) {
			ImGui::PushStyleColor(ImGuiCol_Text, RED_COLOR);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, DEFAULT_COLOR);
		}

		for (int column = 0; column < COLUMN_SIZE(difficulty); column++) {
			ImGui::TableSetColumnIndex(column);
			switch (column) {
			case 0:
				ImGui::Text("%s", diffGrade.second.name.c_str());
				break;
			case 1:
				ImGui::Text("%ld", diffGrade.first);
				break;
			case 2:
				ImGui::Text("%dx%d", diffGrade.second.width, diffGrade.second.height);
				break;
			case 3:
				ImGui::Text("%d", diffGrade.second.numberOfMines);
				break;
			}
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
		ScoreRecord record {
			.score = score,
			.name = parts[1],
		};
		if (difficulty == CUSTOM_DIFFICULTY) {
			record.width = std::stoi(parts[2]);
			record.height = std::stoi(parts[3]);
			record.numberOfMines = std::stoi(parts[4]);
		}

		m_scores[difficulty].insert({score, record});
	}
}

