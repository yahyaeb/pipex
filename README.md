# Pipex

Pipex is a project from the 42 curriculum designed to replicate the behavior of the Unix shell pipeline (`|`). It enables the execution of multiple commands in sequence, where the output of one command serves as the input to the next. Additionally, the bonus part includes support for handling multiple commands and a "here-document" feature.

## Table of Contents
- [Overview](#overview)
- [Usage](#usage)
- [Installation](#installation)
- [Examples](#examples)
- [Credits](#credits)

## Overview
The objective of this project is to enhance the understanding of process creation and management in Unix-like systems. It emphasizes the use of system calls such as `pipe()`, `fork()`, `dup2()`, and `execve()` to implement inter-process communication. The bonus part extends the functionality to handle multiple commands and introduces the "here-document" feature for inline input redirection.

## Usage

The program operates with the following arguments:
```
./pipex file1 cmd1 cmd2 ... cmdN file2
```
- `file1`: Input file.
- `cmd1, cmd2, ..., cmdN`: Commands to execute in sequence.
- `file2`: Output file.

Pipex redirects the content of `file1` to `cmd1`, passes the output through the sequence of commands, and writes the final output to `file2`.

For the "here-document" feature:
```
./pipex here_doc LIMITER cmd1 cmd2 ... cmdN file2
```
- `here_doc`: Activates the here-document mode.
- `LIMITER`: Delimiter to mark the end of the inline input.
- `cmd1, cmd2, ..., cmdN`: Commands to execute in sequence.
- `file2`: Output file.

In this mode, the program reads input from the standard input until the `LIMITER` is encountered, then processes the commands and writes the output to `file2`.

## Installation

1. Clone the repository:
	```bash
	git clone https://github.com/your_username/pipex.git
	cd pipex
	```

2. Compile the program:
	```bash
	make
	```

## Examples

### Example 1
```bash
./pipex infile "ls -l" "wc -l" outfile
```
This command executes `ls -l` on the content of `infile`, pipes the output to `wc -l`, and saves the result in `outfile`.

### Example 2
```bash
./pipex input.txt "grep 42" "sort" "uniq" output.txt
```
This command filters lines containing "42" from `input.txt`, sorts them, removes duplicates, and saves the result in `output.txt`.

### Example 3 (Here-document)
```bash
./pipex here_doc END "cat -e" "grep 42" output.txt
```
This command reads input from the standard input until the word `END` is encountered, processes the input through `cat -e` and `grep 42`, and saves the result in `output.txt`.

## Author

- **Yahia El Boukili** - [GitHub Profile](https://github.com/yahyaeb)