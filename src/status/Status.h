#pragma once

#include "records/PQueue.h"
#include "Layer.h"

#include <cstddef>
#include <fstream>
#include <map>

struct ScoreRecord {
	long score;
	std::string name;
	int width;
	int height;
	int numberOfMines;
	size_t hash;
};

using DifficultyTab = PQueue<ScoreRecord>;

class Status
	: public Layer
{
public:
	enum SortOrder {
		Score,
		Alphabetically,
		NumberOfMines,
		BoardSize,
	};

	static std::shared_ptr<Status> create()
	{
		return std::make_shared<Status>();
	}

	explicit Status();
	void render() override;
	int difficulty() const { return m_difficulty; }

	void onAttach() override;

	void setSortingOrder(SortOrder order);

	~Status();

private:
	std::string difficultyString(int difficulty = -1) const;
	void createTabTable(int difficulty = -1);
	std::vector<std::string> split(const std::string &s, char delim);
	void loadScoreFile();

private:
	int m_difficulty;
	int m_numberOfMines;
	ScoreRecord m_score;
	SortOrder m_sortOrder;

	int m_localHeight;
	int m_localWidth;
	std::fstream m_scoreFile;
	std::map<long, DifficultyTab> m_scores;
	std::string m_name;
};

bool operator==(const ScoreRecord &lhs, const ScoreRecord &rhs);
bool operator>(const ScoreRecord &lhs, const ScoreRecord &rhs);
bool operator<(const ScoreRecord &lhs, const ScoreRecord &rhs);
