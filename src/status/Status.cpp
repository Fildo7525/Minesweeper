#include "Status.h"
#include "imgui.h"

#include <algorithm>

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define DEFAULT_COLOR ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define SCORE_FILE_NAME "scores.txt"

Status::Status(std::shared_ptr<Board> &board)
	: Layer("Status")
	, m_board(board)
	, m_difficulty(0)
	, m_numberOfMines(board->totalNumberOfMines())
	, m_scoreFile(SCORE_FILE_NAME, std::ios::in)
	, m_scores()
{
	if (!m_scoreFile.is_open()) {
		m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);
	}

	int score;
	std::string name;
	while (m_scoreFile >> score >> name) {
		m_scores[score] = name;
	}
}

static long score = 0;
void Status::render()
{
	if (m_board->isGameOver() == Board::GameOverState::Win) {
		score = 100 * (m_board->totalNumberOfCells() * m_numberOfMines) / m_board->numberOfClicks() / m_board->elapsedTime();
		m_scores[score] = "User";
	}

	ImGui::Begin("Game Status", NULL, m_windowFlags);

	ImGui::Text("Mines: %d / %d", m_board->numberOfFlags(), m_board->totalNumberOfMines());
	auto time = m_board->elapsedTime();
	if (time == -1) {
		ImGui::Text("Elapsed time: 0:0");
	}
	else {
		auto minutes = time / 60;
		ImGui::Text("Elapsed time: %2ld:%ld", minutes, time%60);
	}

	{
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
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::BeginChild("ChildR", ImVec2(0, 260), ImGuiChildFlags_Border, window_flags);

			ImGui::LabelText("Max score", "User name");

			for (auto &localScore : m_scores) {
				if (score == localScore.first) {
					ImGui::PushStyleColor(ImGuiCol_Text, RED_COLOR);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Text, DEFAULT_COLOR);
				}

				char buf[32];
				sprintf(buf, "%ld", localScore.first);
				ImGui::LabelText(buf, "%s", localScore.second.c_str());
				ImGui::PopStyleColor();
			}

			ImGui::EndChild();
			ImGui::PopStyleVar();
		}
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
	if (ImGui::InputInt("Mines", &m_numberOfMines)) {
		m_numberOfMines = std::clamp(m_numberOfMines, 1, m_board->totalNumberOfCells()-9);
		m_board->setNumberOfMines(m_numberOfMines);
		m_board->resetTimer();
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

Status::~Status()
{
	m_scoreFile.close();
	m_scoreFile.open(SCORE_FILE_NAME, std::ios::out);

	for (auto &score : m_scores) {
		m_scoreFile << score.first << " " << score.second << '\n';
	}

	m_scoreFile.flush();
	m_scoreFile.close();
}
