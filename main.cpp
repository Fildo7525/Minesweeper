#include "Application.h"
#include "Board.h"
#include "Status.h"
#include <memory>

int main(int argc, char *argv[])
{
	auto app = Application::create({
		"Minesweeper",
		1000,
		720,
		true,
		false,
		true,
		"../font/BitstromWeraNerdFontMono-Regular.ttf"
	});

	std::shared_ptr<Board> board(new Board(10, 10, 20));
	app->addLayer(board);
	app->addLayer(std::make_shared<Status>(board));

	app->run();
	return 0;
}

