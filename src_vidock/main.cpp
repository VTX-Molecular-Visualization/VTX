/*
 * File: main.cpp
 * Author: Maxime MARIA
 */

#include <iostream>

#include "app.hpp"

int main(int argc, char *argv[])
{
	try
	{
		vidocklab::App app;
		app.init(argc, argv);
		app.launch();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Exception caught: " << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Good bye !" << std::endl;
	return EXIT_SUCCESS;
}
