# sudoku_solver

Solver for Sudoku puzzles for the command line.

## Use

```
Usage: sudoku_solver.exe [OPTION] [FILE]
  --help                        Show this help.
  --template[=W[xH][,BW[xBH]]]  Print a grid template of dimensions width x height fields of width x height blocks.
  FILE                          File path to read grid from. When FILE is -, read standard input.
```

Printout and input parser uses ASCII terminal or text file.
Suitable empty-grid templates of arbitrary grid and block size can be printed and piped to output file.

Afterwards, fill in your Sudoku's givens with the help of any text editor.

Then paste or read back the filled-out grid file to put the solver to work.
For example, the input (with 4x4 size and 2x2 block size)
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
The solver will print the result to the command line in the same format.

Sudoku grids of higher-than normal complexity can be given, too, e.g. [a 12x12,4x3 grid with digits 0...B](test/reference_12x12,4x3.txt).

## Build

Uses [lexy parser combinator library](https://lexy.foonathan.net/) for parsing input and [hypervector](https://github.com/mporsch/hypervector) for storage.

Build using CMake.

## Design

Internally, it uses only very basic algorithms that most players intuitively employ and adds some clever guessing in recursion.
Structure-of-arrays rather than array-of-structures is employed for adding annotations and the like to the grid fields.
C++23 ranges are used to zip-iterate the variable-sized and -structured grid according to the Sudoku rules.
