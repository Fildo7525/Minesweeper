#pragma once

#include "Board.h"

class Status
	: public Layer
{
public:
	explicit Status(std::shared_ptr<Board> &board);
	void render() override;
	int difficulty() const { return m_difficulty; }

private:
	std::shared_ptr<Board> m_board;
	int m_difficulty;
	int m_numberOfMines;

	int m_localHeight;
	int m_localWidth;
};
