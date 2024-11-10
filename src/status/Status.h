#pragma once

#include "Board.h"

#include <fstream>
#include <map>

class Status
	: public Layer
{
public:
	explicit Status(std::shared_ptr<Board> &board);
	void render() override;
	int difficulty() const { return m_difficulty; }
	void setNewScore(int score, const std::string &name);

	~Status();

private:
	std::shared_ptr<Board> m_board;
	int m_difficulty;
	int m_numberOfMines;

	int m_localHeight;
	int m_localWidth;
	std::fstream m_scoreFile;
	std::map<long, std::string, std::greater<long>> m_scores;
	std::string m_name;
};
