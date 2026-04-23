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
	auto safe_toupper(char const letter) -> char;
	auto letter_index(char const letter) -> std::size_t;
	auto read_file(std::string const &path) -> std::string;
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

		auto step_inserting(char const letter) -> TrieNode *;
		auto step(char const letter) const -> TrieNode const *;
	};

	class Trie
	{
		TrieNode m_root {};

	public:
		auto insert(std::string const &word) -> void;
		auto root() const -> TrieNode const *;
	};
}






auto main(int argc, char *argv[]) -> int
{
	auto const input_path = std::string(argv[1]);

	std::cout << utils::read_file(input_path);
}







// IMPLEMENTATIONS

auto utils::safe_toupper(char const letter) -> char
{
	return static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
}

auto utils::letter_index(char const letter) -> std::size_t
{
	return safe_toupper(letter) - 'A';
}

auto utils::read_file(std::string const &path) -> std::string
{
	auto file = std::ifstream(path);
	auto buffer = std::ostringstream {};

	buffer << file.rdbuf();

	return buffer.str();
}

auto data::TrieNode::step_inserting(char const letter) -> TrieNode *
{
	auto &child = children[utils::letter_index(letter)];
	if (child == nullptr)
	{
		child = std::unique_ptr<TrieNode>(new TrieNode {});
	}

	return child.get();
}

auto data::TrieNode::step(char const letter) const -> TrieNode const *
{
	return children[utils::letter_index(letter)].get();
}

auto data::Trie::insert(std::string const &word) -> void
{
	auto *current = &m_root;
	for (auto const letter: word)
	{
		current = current->step_inserting(letter);
	}

	current->is_end = true;
}

auto data::Trie::root() const -> TrieNode const *
{
	return &m_root;
}
