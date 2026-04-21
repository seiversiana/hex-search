// SPDX-FileCopyrightText: Copyright (C) Nile Jocson <seiversiana@gmail.com>
// SPDX-License-Identifier: MPL-2.0

#include <fstream>
#include <iostream>
#include <sstream>



auto read_file_to_string(std::string const &path) -> std::string
{
	auto file = std::ifstream(path);
	auto buffer = std::ostringstream {};

	buffer << file.rdbuf();

	return buffer.str();
}



auto main(int argc, char *argv[]) -> int
{
	auto const input_path = std::string(argv[1]);

	std::cout << read_file_to_string(input_path);
}
