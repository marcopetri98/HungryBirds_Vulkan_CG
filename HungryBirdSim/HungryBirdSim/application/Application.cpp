//#include <iostream>
//#include <stdexcept>
//#include <cstdlib>
//
//#include "Application.h"
//
//namespace app
//{
//	Application::Application(string title, int width, int height)
//	{
//		graphicsEngine = GraphicsEngine(title, width, height);
//	}
//	
//	int Application::start()
//	{
//		try
//		{
//			graphicsEngine.run();
//		}
//		catch (const std::exception& e)
//		{
//			std::cerr << e.what() << std::endl;
//			return EXIT_FAILURE;
//		}
//
//		return EXIT_SUCCESS;
//	}
//}