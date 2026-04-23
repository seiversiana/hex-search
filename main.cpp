// SPDX-FileCopyrightText: Copyright (C) Nile Jocson <seiversiana@gmail.com>
// SPDX-License-Identifier: MPL-2.0

#include <cctype>

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>



namespace utils
{
	auto safe_toupper(char const letter) -> char
	{
		return static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
	}

	auto letter_index(char const letter) -> std::size_t
	{
		return safe_toupper(letter) - 'A';
	}
}



namespace data
{
	class Grid
	{

	};

	struct TrieNode
	{
		bool is_end = false;
		std::array<std::unique_ptr<TrieNode>, 26> children {};

		auto step_inserting(char const letter) -> TrieNode *
		{
			auto &child = children[utils::letter_index(letter)];
			if (child == nullptr)
			{
				child = std::unique_ptr<TrieNode>(new TrieNode {});
			}

			return child.get();
		}

		auto step(char const letter) const -> TrieNode const *
		{
			return children[utils::letter_index(letter)].get();
		}
	};

	class Trie
	{
	public:
		auto insert(std::string const &word) -> void
		{
			TrieNode *current = &m_root;
			for (auto const letter: word)
			{
				current = current->step_inserting(letter);
			}

			current->is_end = true;
		}

		auto root() const -> TrieNode const *
		{
			return &m_root;
		}

	private:
		TrieNode m_root {};
	};
}



namespace files
{
	auto read_file(std::string const &path) -> std::string
	{
		auto file = std::ifstream(path);
		auto buffer = std::ostringstream {};

		buffer << file.rdbuf();

		return buffer.str();
	}
}



auto main(int argc, char *argv[]) -> int
{
	auto const input_path = std::string(argv[1]);

	std::cout << files::read_file(input_path);
}
