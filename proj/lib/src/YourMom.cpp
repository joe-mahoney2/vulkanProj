#include "YourMom.h"
/*
    Helper functions and stuff not directly related to the 
    main engine classes and functions go here
*/
std::vector<char> readFile (const std::string& filename) {
	std::ifstream file (filename, std::ios::ate | std::ios::binary);

	if (!file.is_open ()) {
		throw std::runtime_error ("failed to open file!");
	}

	size_t file_size = (size_t)file.tellg ();
	std::vector<char> buffer (file_size);

	file.seekg (0);
	file.read (buffer.data (), static_cast<std::streamsize> (file_size));

	file.close ();

	return buffer;
}