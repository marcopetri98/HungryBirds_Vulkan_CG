#pragma once

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <string>

#include "../graphics/GraphicsEngine.h"

using std::string;
using graphics::DEFAULT_TITLE;
using graphics::DEFAULT_WIDTH;
using graphics::DEFAULT_HEIGHT;
using graphics::GraphicsEngine;

namespace app
{
	class Application
	{
		public:
		Application(string title = DEFAULT_TITLE, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
		/**
		 * Starts the application.
		 * 
		 * The function starts the application by turning on the 
		 * GraphicsEngine.
		 * 
		 */
		int start();

		private:
		GraphicsEngine graphicsEngine;
	};
}

#endif