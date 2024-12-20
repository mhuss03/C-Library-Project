# Library Management System

This project is a **Library Management System**, developed as a learning exercise to enhance programming skills and explore software development concepts. The system includes two versions:

1. **Command-Line Interface (CLI)**
2. **Graphical User Interface (GUI)**

## Overview

The Library Management System is designed to:

- Provide CRUD (Create, Read, Update, Delete) functionality for managing books in the library.

Both versions were built to practice programming techniques, reinforce design principles, and explore different user interface paradigms.

## Features

### CLI Version

- Add new books to the library database.
- Search for books by title, author, or ISBN.
- Issue and return books.
- List all books in the library.
- A simple, text-based menu for navigation.

### GUI Version

- A user-friendly graphical interface for all operations.
- Interactive forms for adding and managing books.
- Dropdown menus for navigation.
- Real-time feedback for actions like issuing or returning books.

## Technology Stack

- **Language**: C
- **CLI**: Standard input/output with libraries like `stdio.h`.
- **GUI**: Built using a C-compatible GUI framework (e.g., GTK or Windows API).
- **Data Storage**: Files for persistent storage.

## Installation

### CLI Version

1. Clone the repository.
2. Navigate to the `cli` directory.
3. Compile the code:
   ```sh
   gcc -o library_cli library_cli.c
   ```
