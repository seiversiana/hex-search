// SPDX-FileCopyrightText: Copyright (C) Nile Jocson <seiversiana@gmail.com>
// SPDX-License-Identifier: MPL-2.0

#include <cctype>

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>



namespace data
{
	class Grid;
	class Trie;
}

namespace utils
{
	auto safe_toupper(char const letter) -> char;

	auto letter_index(char const letter) -> std::size_t;

	auto read_file(std::string const &path) -> std::string;

	template<typename T>
	auto extract(std::istringstream &buffer) -> T;

	auto parse(std::string const &data) -> data::Trie;
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
	auto const input = utils::read_file(input_path);
	auto const trie = utils::parse(input);

	int x = 0;
}







// Implementations

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
	auto stream = std::ostringstream {};

	stream << file.rdbuf();

	return stream.str();
}

template<typename T>
auto utils::extract(std::istringstream &buffer) -> T
{
	T value;
	buffer >> value;
	return value;
}

auto utils::parse(std::string const &data) -> data::Trie
{
	auto stream = std::istringstream(data);

	auto const m = extract<std::size_t>(stream);
	auto const n = extract<std::size_t>(stream);
	auto const w = extract<std::size_t>(stream);

	auto const top_rows = m / 2;
	auto const min_width = n - top_rows;
	auto const top_count = top_rows * (min_width + n - 1) / 2;
	auto const lettercount = 2 * top_count + n;

	for (int i = 0; i < lettercount; ++i)
	{
		extract<char>(stream);
	}

	auto trie = data::Trie {};

	for (int i = 0; i < w; ++i)
	{
		auto const word = extract<std::string>(stream);
		std::cout << word << '\n';
		trie.insert(word);
	}

	return trie;
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
