#include "Application.h"
#include "Board.h"
#include "Status.h"
#include "imgui.h"

#include <execution>
#include <filesystem>
#include <iostream>

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define DEFAULT_COLOR ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define SCORE_FILE_NAME "scores.txt"
#define INDENT_CUSTOM_SIZE 25
#define MAX_WIDTH 50
#define MAX_HEIGHT 40
#define MIN_SIZE 9
#define CUSTOM_DIFFICULTY 3
#define MAX_NAME_SIZE 32
#define COLUMN_SIZE(dif) (dif == CUSTOM_DIFFICULTY ? 4 : 2)

Status::Status()
	: Layer("Status")
	, m_difficulty(0)
	, m_numberOfMines()
	, m_scoreFile(SCORE_FILE_NAME, std::ios::in)
	, m_scores()
	, m_name("User")
	, m_sortOrder(SortOrder::Score)
{
	if (!m_scoreFile.is_open()) {
		std::cout << "Creating new score file" << std::endl;
		m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);
		m_scoreFile << (int)m_sortOrder << '\n' << "0\n1\n2\n3" << std::endl;
		m_scoreFile.close();
	}

	if (std::filesystem::is_empty(SCORE_FILE_NAME)) {
		m_scoreFile.close();
		m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);
		m_scoreFile << m_sortOrder << "\n0\n1\n2\n3" << std::endl;
		m_scoreFile.close();
	}

	loadScoreFile();
	m_name.resize(MAX_NAME_SIZE);
}

void Status::render()
{
	auto board = app().getLayer<Board>("Board");
	if (board == nullptr) {
		throw std::runtime_error("Could not find board layer");
	}

	if (board->isGameOver() == Board::GameOverState::Win) {
		board->ackGameOver();
		m_score.score = (board->totalNumberOfCells() * m_numberOfMines - board->elapsedTime()) / board->numberOfClicks();
		m_score.name = m_name;
		m_score.width = m_localWidth;
		m_score.height = m_localHeight;
		m_score.numberOfMines = m_numberOfMines;

		auto now = std::chrono::system_clock::now();
		auto time = now.time_since_epoch().count();
		m_score.hash = std::hash<std::string>()(m_name) ^ std::hash<long>()(time) ^ std::hash<long>()(m_score.score);

		m_scores[m_difficulty].push(m_score);
	}

	ImGui::Begin("Game Status", NULL, m_windowFlags);

	if (!ImGui::BeginMainMenuBar()) {
		throw std::runtime_error("Could not create main menu bar");
	}

	if (ImGui::BeginMenu("Sort leaderboard")) {

		if (ImGui::MenuItem("Score", "", m_sortOrder == Status::SortOrder::Score)) {
			setSortingOrder(Status::SortOrder::Score);
		}
		if (ImGui::MenuItem("Alphabetically", "", m_sortOrder == Status::SortOrder::Alphabetically)) {
			setSortingOrder(Status::SortOrder::Alphabetically);
		}
		if (ImGui::MenuItem("Number of mines", "", m_sortOrder == Status::SortOrder::NumberOfMines)) {
			setSortingOrder(Status::SortOrder::NumberOfMines);
		}
		if (ImGui::MenuItem("Board size", "", m_sortOrder == Status::SortOrder::BoardSize)) {
			setSortingOrder(Status::SortOrder::BoardSize);
		}
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	ImGui::InputText("Player name", m_name.data(), MAX_NAME_SIZE);

	ImGui::Text("Mines: %d / %d", board->numberOfFlags(), board->totalNumberOfMines());
	auto time = board->elapsedTime();
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

		m_numberOfMines = board->totalNumberOfMines();

		if (m_difficulty != CUSTOM_DIFFICULTY) {
			board->setDifficulty(i);
		}
		else {
			board->setOnlyDifficulty(i);
			m_localHeight = board->height();
			m_localWidth = board->width();
		}
	}
	if (m_difficulty == 3) {
		float statusWidth = ImGui::GetWindowWidth();
		ImGui::PushItemWidth(statusWidth / 4.);

		ImGui::Indent(INDENT_CUSTOM_SIZE);
		if (ImGui::InputInt("Width", &m_localWidth))
			m_localWidth = std::clamp(m_localWidth, MIN_SIZE, MAX_WIDTH);

		/* ImGui::SameLine(); */
		if (ImGui::InputInt("Height", &m_localHeight))
			m_localHeight = std::clamp(m_localHeight, MIN_SIZE, MAX_HEIGHT);

		/* ImGui::SameLine(); */
		if (ImGui::Button("Apply")) {

			// Limit the width and height
			board->width() = m_localWidth;
			board->height() = m_localHeight;

			board->setupEmptyTiles();
			m_numberOfMines = (m_localWidth * m_localHeight) / 5;
			board->setNumberOfMines(m_numberOfMines);
			board->resetTimer();
		}
		ImGui::Unindent(INDENT_CUSTOM_SIZE);

		ImGui::PopItemWidth();
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
	if (ImGui::InputInt("Mines", &m_numberOfMines)) {
		m_numberOfMines = std::clamp(m_numberOfMines, 5, board->totalNumberOfCells()-9);
		board->setNumberOfMines(m_numberOfMines);
		board->resetTimer();
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
		board->on_refreshBoard_activated();
	}

	ImGui::PopStyleColor(3);

	ImGui::End();
}

void Status::onAttach()
{
	auto board = app().getLayer<Board>("Board");
	m_numberOfMines = board->totalNumberOfMines();
}

void Status::setSortingOrder(SortOrder order)
{
	m_sortOrder = order;
	std::function<bool(const ScoreRecord &, const ScoreRecord &)> compare;

	switch (m_sortOrder) {
		case SortOrder::Score:
			compare = std::greater<ScoreRecord>();
			break;
		case SortOrder::Alphabetically:
			compare = [](const ScoreRecord &lhs, const ScoreRecord &rhs) {
				if (lhs.name == rhs.name) {
					return lhs.score > rhs.score;
				}

				return lhs.name < rhs.name;
			};
			break;
		case SortOrder::NumberOfMines:
			compare = [](const ScoreRecord &lhs, const ScoreRecord &rhs) {
				if (lhs.numberOfMines == rhs.numberOfMines) {
					return lhs.score > rhs.score;
				}

				return lhs.numberOfMines > rhs.numberOfMines;
			};
			break;
		case SortOrder::BoardSize:
			compare = [](const ScoreRecord &lhs, const ScoreRecord &rhs) {
				if (lhs.width == rhs.width && lhs.height == rhs.height) {
					return lhs.score > rhs.score;
				}

				return lhs.width*lhs.height > rhs.width*rhs.height;
			};
			break;
		default:
			compare = std::greater<ScoreRecord>();
			break;
	}

	std::for_each(std::execution::par_unseq, m_scores.begin(), m_scores.end(), [compare](auto &score) {
		score.second.setSorter(compare);
	});
}

Status::~Status()
{
	m_scoreFile.close();
	m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);

	m_scoreFile << (int)m_sortOrder << '\n';

	for (auto &score : m_scores) {
		// Printing difficulty
		m_scoreFile << score.first << '\n';
		for (auto &record : score.second) {
			// printing score and name
			m_scoreFile << record.score << " " << record.name << " " << record.hash;

			if (score.first == CUSTOM_DIFFICULTY) {
				// printing width and height
				m_scoreFile << " " << record.width << " " << record.height;
				// printing number of mines
				m_scoreFile << " " << record.numberOfMines;
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

		if (m_score == diffGrade) {
			ImGui::PushStyleColor(ImGuiCol_Text, RED_COLOR);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Text, DEFAULT_COLOR);
		}

		for (int column = 0; column < COLUMN_SIZE(difficulty); column++) {
			ImGui::TableSetColumnIndex(column);
			switch (column) {
			case 0:
				ImGui::Text("%s", diffGrade.name.c_str());
				break;
			case 1:
				ImGui::Text("%ld", diffGrade.score);
				break;
			case 2:
				ImGui::Text("%dx%d", diffGrade.width, diffGrade.height);
				break;
			case 3:
				ImGui::Text("%d", diffGrade.numberOfMines);
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
	std::string line;
	int difficulty = 0;

	if (!m_scoreFile.is_open()) {
		m_scoreFile.open(SCORE_FILE_NAME, std::ios::in);
	}

	std::getline(m_scoreFile, line);
	auto order = static_cast<SortOrder>(std::stoi(line));

	while (std::getline(m_scoreFile, line)) {
		auto parts = split(line, ' ');
		if (parts.size() == 1) {
			difficulty = std::stoi(parts[0]);
			m_scores[difficulty] = {};
			continue;
		}

		ScoreRecord record {
			.score = std::stoi(parts[0]),
			.name = std::move(parts[1]),
			.hash = std::stoul(parts[2]),
		};

		if (difficulty == CUSTOM_DIFFICULTY) {
			record.width = std::stoi(parts[3]);
			record.height = std::stoi(parts[4]);
			record.numberOfMines = std::stoi(parts[5]);
		}

		m_scores[difficulty].push(record);
	}

	setSortingOrder(order);
}

bool operator==(const ScoreRecord &lhs, const ScoreRecord &rhs)
{
	return lhs.hash == rhs.hash;
}

bool operator>(const ScoreRecord &lhs, const ScoreRecord &rhs)
{
	return lhs.score > rhs.score;
}

bool operator<(const ScoreRecord &lhs, const ScoreRecord &rhs)
{
	return lhs.score < rhs.score;
}
