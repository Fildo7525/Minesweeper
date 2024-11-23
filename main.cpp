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

	app->addLayer(std::make_shared<Board>(10, 10, 20));
	app->addLayer(std::make_shared<Status>());

	app->run();
	return 0;
}

