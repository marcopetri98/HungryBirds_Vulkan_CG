#pragma once

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <string>
#include <chrono>

#include "../RecSolver.h"
#include "../graphics/GraphicsEngine.h"
#include "../physics/PhysicsEngine.hpp"
#include "../graphics/DummyRecursionSolver.hpp"

using Clock = std::chrono::high_resolution_clock;
using std::string;
using graphics::GraphicsEngine;
using physics::PhysicsEngine;

namespace app
{
	class Application
	{
		public:
		Application(string title = "Vulkan application", int width = 800, int height = 600);
		/**
		 * Starts the application.
		 * 
		 * The function starts the application by turning on the 
		 * GraphicsEngine.
		 * 
		 */
		int start();
		/**
		 * This function is called at each frame and must handle all the game changes between frames. Given that deltaTime is the time elapsed between the last and the current frame, everything can be computed without using Vulkan..
		 * 
		 * @param deltaTime time elapsed between last and current frame
		 */
		void update(float deltaTime);
		/**
		 * This function is called to handle user's commands through the keyboard.
		 * 
		 * @param deltaTime time elapsed between last and current frame
		 */
		void handleCommands(float deltaTime);

		private:
		GraphicsEngine* graphicsEngine;
		PhysicsEngine* physicsEngine;
		GameObject* arrow;
		GameObject* bird;
		std::chrono::time_point<Clock> lastCameraChangeTime;
		float arrowAngle1, arrowAngle2;
		bool launched;
	};
}

#endif