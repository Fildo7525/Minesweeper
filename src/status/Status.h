#pragma once

#include "records/DynamicPriorityQueue.h"
#include "Layer.h"

#include <cstddef>
#include <fstream>
#include <map>
#include <print>

struct ScoreRecord {
	long score;
	std::string name;
	int width;
	int height;
	int numberOfMines;
	size_t hash;
};

using DifficultyTab = DynamicPriorityQueue<ScoreRecord>;

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

template <>
struct std::formatter<ScoreRecord> {
	constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const ScoreRecord& record, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "{} {} {} {}", record.score, record.name, record.numberOfMines, record.hash);
	}
};
