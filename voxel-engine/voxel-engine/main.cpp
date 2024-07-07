#include "voxel_engine.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(void) {
	voxel_engine app{};
	
	try {
		app.run_app();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}