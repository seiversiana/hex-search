// SPDX-FileCopyrightText: Copyright (C) Nile Jocson <seiversiana@gmail.com>
// SPDX-License-Identifier: MPL-2.0

#import "@preview/charged-ieee:0.1.4": ieee

#import "@preview/fletcher:0.5.8": diagram, node, edge
#import "@preview/unify:0.7.1": num, qty, qtyrange,

#show: ieee.with(
	title: "Hexagonal Grid Word Search",
	authors: (
		(
			name: "Nile Jocson",
			department: [Electrical and Electronics Engineering Institute],
			organization: [University of the Philippines Diliman],
			location: [Quezon City, Philippines],
			email: "nile.xavier.jocson@eee.upd.edu.ph"
		),
	),
	bibliography: bibliography("refs.yaml"),
	figure-supplement: [Fig.],
)

#show raw: set text(font: "DejaVu Sans Mono", size: 0.8em)

#set figure(placement: top)

#set table(
	columns: (6em, auto),
	align: (left, right),
	inset: (x: 8pt, y: 4pt),
	stroke: (x, y) => if y <= 1 { (top: 0.5pt) },
	fill: (x, y) => if y > 0 and calc.rem(y, 2) == 0  { rgb("#efefef") },
)



= Source
The Git repository for this project is located at https://github.com/seiversiana/hex-search.
Included are the license and source code.



= The Program
The program starts with the `main()` function. This takes in the command-line
argument count and list from the OS. The input file path is taken from the list
and then its contents are fully read into a string using `utils::read_file()`.
That string is then parsed by `utils::parse()` into the two main datastructures,
`data::Grid` and `data::Trie`. All input words are then searched in the
hexagonal grid using `data::Grid::search_words()` and passing in the parsed
trie. The result from this function is an `std::set<std::string>` containing
all words found. The number of words and the list of words are outputted, and
then the program exits.



= The Hexagonal Grid
== Justification
The hexagonal grid is internally represented using a `vector<vector<char>>` as
it allows $O(1)$ indexing. The grid is also only constructed once at the start
of the program, so resizing is not an issue, nor is insertion and deletion,
which are $O(n)$ worst-case.

== Instantiation
In order to constructed the hexagonal grid, its internal `vector<vector<char>>`
representation needs to be instantiated. This nested vector represents the rows,
which then includes the cells of the grid.

The specification details a hexagonal grid where each row decrements in width
with respect to its distance from the (one, since $M$ is odd) middle row. Recall
that $M$ is the number of rows and $N$ is the width of the largest row. This
means that:

$
	w = N - d_"mid"
$

The distance from the middle row is determined using the row index and the
middle row index:

$
	d_"mid" = |i_"mid" - i_"row"|
$

And the index of the middle row is determined using $M$:

$
	i_"mid" = floor(M / 2)
$

The grid constructor loops through $0$ and $M$, and instantiates each internal
row vector using the width calculated using the iteration variable $i_"row"$.
The constructed grid solely contains all null characters.

== Population
The hexagonal grid is then populated using the `data::Grid::populate()`
function, which takes in a `vector<char>`. Assuming that this input vector is
equal in size with the number of cells in the grid, the vector is used to
populate the grid from left to right, and top to bottom. This is done using a
simple nested loop.

== Coordinate System
In order to make the neighbor search trivial, the indexing of each grid cell is
done using a double-width coordinate system, detailed in @hexagons. This
massively simplifies the logic of determining the neighbor coordinates, by
eliminating the alternating odd-even logic of regular indexing.

However, converting between double-width coordinates `data::Coords` and array
indices `data::Indices` are still necessary to interface with the internal
vector representation. This is done using `data::Grid::to_coords()` and
`data::Grid::to_indices()`.

Let the row and column of the cell represented in double-width coordinates and
indices form be $(c_r, c_c)$ and $(i_r, i_c)$ respectively.
`data::Grid::to_coords()` is implemented using:

$
	(c_r, c_c) = (i_r, 2 i_c + d_"mid")
$

`data::Grid::to_indices()` is simply the inverse of this function:

$
	(i_r, i_c) = (c_r, (c_c - d_"mid")/2)
$

== Indexing
Indexing is done using the `data::Grid::at()` function. This takes in a
`data::Coords` and outputs a pointer to the indexed character, or null if the
coordinates is out-of-bounds of the Grid. This simply converts the coordinates
to array indices and indexes the internal vector using them.

== Neighbor Search
Listing the coordinates of the neighbors of a cell is done using the
`data::Grid::neighbors()` function. This returns the coordinates of all six
neighbors of a cell:

$
	c_"TL" = (c_r - 1, c_c - 1) \
	c_"TR" = (c_r - 1, c_c + 1) \
	c_"L"  = (c_r    , c_c - 2) \
	c_"R"  = (c_r    , c_c + 2) \
	c_"BL" = (c_r + 1, c_c - 1) \
	c_"BR" = (c_r + 1, c_c + 1)
$

Note that, since `data::Coords` uses `std::size_t` internally, this function
may return underflowed values. This is okay since any underflow will be flagged
as an out-of-bounds index by `data::Grid::at()`. Recall that the underflow of
an unsigned integer will simply wrap around to the maximum value of the type
(`SIZE_MAX` in our case) in C++. The maximum array index used in this project is
16, so this is a non-issue.



= The Prefix Tree
== Justification
A trie or prefix tree was used for the word list as it allows one-by-one stepping
through the tree for each letter, which is needed later in the word search
algorithm. The tree features an $O(L)$ insertion, where $L$ is the number of
letters in the word to be inserted. Finding a valid child node in a node is also
$O(1)$ as this is done by using the letter's index in the alphabet to index an
array.

== Trie Nodes
The nodes of a trie are represented by the `data::TrieNode` struct. This
includes an indicator for the end of a word, and a 26-element array of pointers
to child trie nodes, where each index represents a letter. The index of a letter
is retrieved using `utils::letter_index()`, which does a simple subtraction:

$
	i_l = "upper"(l) - "'A'"
$

Stepping through each trie node is done by the `data::TrieNode::step()`
function. Given a letter, this returns the child node that represents that
letter, or null if it doesn't exist.

In order to easily construct a trie, the `data::TrieNode::step_inserting()`
function is provided. This has the same functionality as the
`data::TrieNode::step()` function, except that it creates the child node for the
letter if it doesn't exist, and returns that instead of null.

== The Trie
The trie is represented by the `data::Trie` class. This includes an internal
`data::TrieNode` representing the root node of the trie. Inserting a word into
the trie is done by the `data::Trie::insert()` function, which loops through
an input word while step-inserting into the root node.

A search function is not provided. Instead, the caller is expected to search
the trie letter-by-letter by using the root node retrieved by
`data::Trie::root()`. This is ideal for the word search algorithm used in this
project.



= The Search Algorithm
== Driver Function
Driving the search algorithm is done by the `data::Grid::search_words()`
function. This takes in a `data::Trie` and returns an `std::set<std::string>`
which includes all words found by the algorithm in the grid. An `std::set`
was used here as it automatically orders its elements (alphabetically for
`std::string`) and eliminates duplicate elements.

The driver function sets up all initial conditions for the DFS to work; this
includes the visited `std::vector<std::vector<bool>>` which matches each index
of the grid, the current word `std::string` which holds the current word
represented by all trie nodes at that point in recursion, and the results
`std::set<std::string>` which holds all found words. It then calls the DFS for
all cells in the grid, with the root node of the trie.

== Depth-first Search Function
The recursive depth-first search is done by the `data::Grid::dfs()` function.
This takes in the current coordinates, the current trie node, the current
visited nodes, the current constructed word, and a reference to the results set.

First, it checks if the cell at the specified coordinates exists, and stops if
it doesn't. This eliminates all out-of-bounds coordinates generated by the
`data::Grid::neighbors()` function.

Second, it checks if the cell has been visited before in the current path, and
stops if it has. This prevents any loops in the path. This operation is $O(1)$,
as it indexes the visited array using the coordinates converted to array
indices.

Third, it checks if the current trie node has the letter inside the cell as a
child, and stops if it doesn't. This is also $O(1)$ as the number of children
of a trie node is constant at 26. The child node is then set as the new node.

Fourth, it sets the cell as visited for the current path and adds the current
letter to the current word. If the new node has the end flag set, the word is
inserted into the results set.

Then, the function is recursed for each neighbor coordinates of the current
cell, with the new node. The visited, current word, and results parameter stay
the same, as in, they point to the same objects.

== Time and Space Complexities
The worst-case time complexity of the entire search algorithm is $O(N_C dot 6^L)$,
where $N_C$ is the number of cells and $L$ is the longest word in the trie. This
worst case is for when no prefixes are pruned in the search (when the trie node
check never gives null).

The worst-case space complexity is $O(N_R)$, where $N_R$ is the recursion depth,
as the number of allocations in the algorithm grow linearly with the depth of
recursion.
