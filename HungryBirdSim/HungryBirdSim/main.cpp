#include "application/Application.h"

using app::Application;

int main()
{
	Application app = Application("Hungry birds simulator");

	app.start();
}