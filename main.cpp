// SPDX-FileCopyrightText: Copyright (C) Nile Jocson <seiversiana@gmail.com>
// SPDX-License-Identifier: MPL-2.0

#include <cctype>
#include <cstdlib>

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <utility>
#include <vector>



namespace data
{
	struct TrieNode;
	struct Trie;
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
	struct Indices
	{
		std::size_t r;
		std::size_t c;
	};

	// Grid system uses doubled-width coordinates
	struct Coords
	{
		std::size_t r;
		std::size_t c;
	};

	class Grid
	{
		std::vector<std::vector<char>> m_grid;

	public:
		Grid(std::size_t const rows, std::size_t const max_width);

		auto populate(std::vector<char> const &letters) -> void;

		auto search_words(Trie const &trie) const -> std::set<std::string>;

		auto to_coords(Indices const &indices) const -> Coords;
		auto to_indices(Coords const &coords) const -> Indices;

		auto at(Coords const &coords) const -> char const *;
		auto neighbors(Coords const &coords) const -> std::vector<Coords>;

		auto num_cells() const -> std::size_t;
		auto distance_from_middle(std::size_t const r) const -> std::size_t;

		auto data() const -> std::vector<std::vector<char>> const &;

	private:
		auto dfs
		(
			Coords const &coords,
			TrieNode const *node,
			std::vector<std::vector<bool>> &visited,
			std::string &current,
			std::set<std::string> &results
		) const -> void;
	};

	struct TrieNode
	{
		bool is_end = false;
		std::array<std::unique_ptr<TrieNode>, 26> children {};

	public:
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
	auto parsed = utils::parse(input);

	auto const grid = std::move(parsed.grid);
	auto const trie = std::move(parsed.trie);

	auto const results = grid.search_words(trie);

	std::cout << results.size() << '\n';
	for (auto const word: results)
	{
		std::cout << word << '\n';
	}
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
		letter = safe_toupper(extract<char>(stream));
	}
	grid.populate(letters);

	auto trie = data::Trie {};
	for (auto i = std::size_t { 0 }; i < num_words; ++i)
	{
		trie.insert(extract<std::string>(stream));
	}

	return data::Parsed { std::move(grid), std::move(trie) };
}



data::Grid::Grid(std::size_t const rows, std::size_t const max_width) :
	m_grid(rows)
{
	for (auto i = std::size_t { 0 }; i < rows; ++i)
	{
		auto const width = max_width - distance_from_middle(i);
		m_grid[i].resize(width);
	}
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

auto data::Grid::search_words(Trie const &trie) const -> std::set<std::string>
{
	auto results = std::set<std::string> {};

	auto visited = std::vector<std::vector<bool>>(m_grid.size());
	for (auto i = std::size_t { 0 }; i < m_grid.size(); ++i)
	{
		visited[i].resize(m_grid[i].size(), false);
	}

	auto current = std::string {};

	for (auto r = std::size_t { 0 }; r < m_grid.size(); ++r)
	{
		for (auto c = std::size_t { 0 }; c < m_grid[r].size(); ++c)
		{
			auto coords = to_coords({ r, c });
			dfs(coords, trie.root(), visited, current, results);
		}
	}

	return results;
}

auto data::Grid::to_coords(Indices const &indices) const -> Coords
{
	return
	{
		indices.r,
		indices.c * 2 + distance_from_middle(indices.r)
	};
}

auto data::Grid::to_indices(Coords const &coords) const -> Indices
{
	return
	{
		coords.r,
		(coords.c - distance_from_middle(coords.r)) / 2
	};
}

auto data::Grid::at(Coords const &coords) const -> char const *
{
	auto const indices = to_indices(coords);

	auto const r = indices.r;
	auto const c = indices.c;

	if (r < m_grid.size() && c < m_grid[r].size())
	{
		return &m_grid[r][c];
	}
	else
	{
		return nullptr;
	}
}

auto data::Grid::neighbors(Coords const &coords) const -> std::vector<Coords>
{
	return std::vector<Coords>
	{
		{ coords.r - 1, coords.c - 1 }, // top-left
		{ coords.r - 1, coords.c + 1 }, // top-right
		{ coords.r    , coords.c - 2 }, // left
		{ coords.r    , coords.c + 2 }, // right
		{ coords.r + 1, coords.c - 1 }, // bottom-left
		{ coords.r + 1, coords.c + 1 }, // bottom-right
	};
}

auto data::Grid::num_cells() const -> std::size_t
{
	auto accum = 0;
	for (auto const &row: m_grid)
	{
		accum += row.size();
	}

	return accum;
}

auto data::Grid::distance_from_middle(std::size_t const r) const -> std::size_t
{
	auto const middle_index = m_grid.size() / 2;

	return (middle_index > r) ? (middle_index - r) : (r - middle_index);
}

auto data::Grid::data() const -> std::vector<std::vector<char>> const &
{
	return m_grid;
}

auto data::Grid::dfs
(
	Coords const &coords,
	TrieNode const *node,
	std::vector<std::vector<bool>> &visited,
	std::string &current,
	std::set<std::string> &results
) const -> void
{
	// Check if neighbor exists
	auto const *letter_ptr = at(coords);
	if (letter_ptr == nullptr)
	{
		return;
	}

	// Check if cell has been visited
	auto const indices = to_indices(coords);
	auto const r = indices.r;
	auto const c = indices.c;
	if (visited[r][c])
	{
		return;
	}

	// Check if the trie node has the letter as a child
	auto const letter = *letter_ptr;
	node = node->step(letter);
	if (node == nullptr)
	{
		return;
	}

	// Set visited for the cell, push the letter into the word, push the word
	// into the results if the trie node is a word end
	visited[r][c] = true;
	current.push_back(letter);
	if (node->is_end)
	{
		results.insert(current);
	}

	// Recurse for all neighbors
	for (auto const &neighbor: neighbors(coords))
	{
		dfs(neighbor, node, visited, current, results);
	}

	// Revert state for backtracking
	visited[r][c] = false;
	current.pop_back();
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
