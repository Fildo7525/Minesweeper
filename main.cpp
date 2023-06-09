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
		"/usr/share/fonts/BS/BitstromWeraNerdFont-Regular.ttf"
	});

	app->addLayer(std::make_shared<Board>(10, 10, 30));

	app->run();
	return 0;
}

