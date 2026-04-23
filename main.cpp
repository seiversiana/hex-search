// SPDX-FileCopyrightText: Copyright (C) Nile Jocson <seiversiana@gmail.com>
// SPDX-License-Identifier: MPL-2.0

#include <cctype>
#include <cstdlib>

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>



namespace data
{
	struct Parsed;
}

namespace utils
{
	auto safe_toupper(char const letter) -> char;

	auto letter_index(char const letter) -> std::size_t;

	auto read_file(std::string const &path) -> std::string;

	template<typename T>
	auto extract(std::istringstream &buffer) -> T;

	auto parse(std::string const &data) -> data::Parsed;
}



namespace data
{
	class Grid
	{
		std::vector<std::vector<char>> m_grid;

	public:
		Grid(std::size_t const rows, std::size_t const max_width);

		auto num_cells() const -> int;
		auto populate(std::vector<char> const &letters) -> void;
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

	struct Parsed
	{
		Grid grid;
		Trie trie;
	};
}






auto main(int argc, char *argv[]) -> int
{
	auto const input_path = std::string(argv[1]);
	auto const input = utils::read_file(input_path);
	auto const trie = utils::parse(input);
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

auto utils::parse(std::string const &data) -> data::Parsed
{
	auto stream = std::istringstream(data);

	auto const rows = extract<std::size_t>(stream);
	auto const max_width = extract<std::size_t>(stream);
	auto const num_words = extract<std::size_t>(stream);

	auto grid = data::Grid(rows, max_width);
	auto letters = std::vector<char>(grid.num_cells());
	for (auto &letter: letters)
	{
		letter = extract<char>(stream);
	}
	grid.populate(letters);

	auto trie = data::Trie {};
	for (int i = 0; i < num_words; ++i)
	{
		trie.insert(extract<std::string>(stream));
	}

	return data::Parsed { std::move(grid), std::move(trie) };
}



data::Grid::Grid(std::size_t const rows, std::size_t const max_width) :
	m_grid(rows)
{
	auto const middle_index = rows / 2;

	for (auto i = std::size_t { 0 }; i < rows; ++i)
	{
		auto const distance = (middle_index > i)
			? (middle_index - i)
			: (i - middle_index);

		auto const width = max_width - distance;
		m_grid[i].resize(width);
	}
}

auto data::Grid::num_cells() const -> int
{
	auto accum = 0;
	for (auto const &row: m_grid)
	{
		accum += row.size();
	}

	return accum;
}

auto data::Grid::populate(std::vector<char> const &letters) -> void
{
	auto i = std::size_t { 0 };
	for (auto &row: m_grid)
	{
		for (auto &cell: row)
		{
			cell = letters[i];
			++i;
		}
	}
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
