#include "Status.h"
#include "imgui.h"

Status::Status(std::shared_ptr<Board>  &board)
	: Layer("Status")
	, m_board(board)
	, m_difficulty(0)
{

}

void Status::render()
{
	ImGui::Begin("Game Status", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	ImGui::Text("Mines: %d / %d", m_board->numberOfFlags(), m_board->totalNumberOfMines());
	auto minutes = m_board->elapsedTime() / 60;
	ImGui::Text("Elapsed time: %2ld:%ld", minutes, m_board->elapsedTime()%60);

	{
		if (ImGui::RadioButton("Easy", &m_difficulty, 0)) {
			m_board->setDifficulty(0);
		}
		if (ImGui::RadioButton("Medium", &m_difficulty, 1)) {
			m_board->setDifficulty(1);
		}
		if (ImGui::RadioButton("Hard", &m_difficulty, 2)) {
			m_board->setDifficulty(2);
		}
	}

	ImGui::End();
}

