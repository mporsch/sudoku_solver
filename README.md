# sudoku_solver
Playground for sudoku puzzles.

## Use

Printout and input parser uses ASCII terminal or text file.

For example the input
```
+~~~+~~~+~~~+~~~+
|   : 4 | 2 :   |
+---+---+---+---+
| 2 :   |   : 3 |
+~~~+~~~+~~~+~~~+
| 1 :   |   : 4 |
+---+---+---+---+
|   : 3 | 1 :   |
+~~~+~~~+~~~+~~~+
```
may be parsed from file or can be given via stdin with copy-paste or piped input.

Suitable empty grid templates can be created and piped to output file. Fill in the Sudoku givens with the help of any text editor.

## Build

Uses [lexy parser combinator library](https://lexy.foonathan.net/) for parsing input and [hypervector](https://github.com/mporsch/hypervector) for storage.

Build using CMake.
