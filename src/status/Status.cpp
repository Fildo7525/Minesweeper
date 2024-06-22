#include "Status.h"
#include "imgui.h"

#define RED_COLOR ImVec4(1.0f, 0.0f, 0.0f, 1.0f)

Status::Status(std::shared_ptr<Board> &board)
	: Layer("Status")
	, m_board(board)
	, m_difficulty(0)
	, m_numberOfMines(board->totalNumberOfMines())
{
}

void Status::render()
{
	ImGui::Begin("Game Status", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

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
	}

	if (ImGui::InputInt("Number of Mines", &m_numberOfMines)) {
		m_numberOfMines = m_numberOfMines < 1 ? 1 : m_numberOfMines;
		m_numberOfMines = m_numberOfMines >= m_board->totalNumberOfCells()-9 ? m_board->totalNumberOfCells()-9 : m_numberOfMines;
		m_board->setNumberOfMines(m_numberOfMines);
		m_board->resetTimer();
	}

	ImGui::End();
}

