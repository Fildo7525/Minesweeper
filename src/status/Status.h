#pragma once

#include "Board.h"

#include <fstream>
#include <map>

struct ScoreRecord {
	long difficulty;
	long score;
	std::string name;
	int width;
	int height;
	int numberOfMines;
};

// TODO: Use priority queue sorted by ScoreRecord.score instead of map.
// This will result into more versatile solution that can be used
// for different sorting orders.
using DifficultyTab = std::map<long, ScoreRecord, std::greater<long>>;

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
	std::string difficultyString(int difficulty = -1) const;
	void createTabTable(int difficulty = -1);
	std::vector<std::string> split(const std::string &s, char delim);
	void loadScoreFile();

private:
	std::shared_ptr<Board> m_board;
	int m_difficulty;
	int m_numberOfMines;
	ScoreRecord m_score;

	int m_localHeight;
	int m_localWidth;
	std::fstream m_scoreFile;
	std::map<long, DifficultyTab> m_scores;
	std::string m_name;
};

bool operator==(const ScoreRecord &lhs, const ScoreRecord &rhs);
