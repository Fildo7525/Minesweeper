#include "Status.h"
#include "imgui.h"

Status::Status(std::shared_ptr<Board>  &board)
	: Layer("Status")
	, m_board(board)
{

}

void Status::render()
{
	ImGui::Begin("Game Status");

	ImGui::Text("Mines: %d / %d", m_board->numberOfFlags(), m_board->totalNumberOfMines());
	// ImGui::Text("Elapsed time: %d", m_board->elapsedTime());

	ImGui::End();
}

