#include "Application.h"
#include "Board.h"

int main(int argc, char *argv[])
{
	auto app = Application::create({
		"Minesweeper",
		1200,
		720,
		true,
		false,
		true,
		"../font/BitstromWeraNerdFontMono-Regular.ttf"
	});

	app->addLayer(std::make_shared<Board>(10, 10, 20));

	app->run();
	return 0;
}

