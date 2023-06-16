#pragma once

#include "Board.h"

class Status
	: public Layer
{
public:
	explicit Status(std::shared_ptr<Board> &board);
	void render() override;

private:
	std::shared_ptr<Board> m_board;
};
