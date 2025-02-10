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

	app->addLayer(Board::create(10, 10, 20));
	app->addLayer(Status::create());

	return app->run();
}

