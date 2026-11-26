# The Locker Heist

A console detective game built with Object-Oriented Programming in C++.

## Team
- Rehab Fatima
- Fatima Saqib

## Build and Run
1. Open the project in Visual Studio (C++ Console Project) with `heist.h`, `heist.cpp` and `main.cpp`.
2. Keep `rules.txt`, `clue.txt`, `locations.txt`, `characters.txt`, `about.txt` and the `leader/` folder (with `scoreBoard.txt`) in the project folder (the one that contains `heist.cpp`).
3. Press `Ctrl + F5` to run the program.

## Project Structure
- `heist.h` - Declarations (enums, structs, exceptions, template and classes).
- `heist.cpp` - Implementation.
- `main.cpp` - Driver program that catches all `GameException` objects.
- `*.txt`, `leader/scoreBoard.txt` - Game data and persistent score board.

## Known Limitations
- The screen is cleared using `system("cls")`, which works on Windows consoles only.
- Quitting a game midway is not recorded on the score board.
- `leader/scoreBoard.txt` must already exist; the program cannot create folders.
- Character data is read from `characters.txt`; there is no in-game editor.
