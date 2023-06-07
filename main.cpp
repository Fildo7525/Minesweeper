#include "Application.h"

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

	app->run();
	return 0;
}

