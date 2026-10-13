# Code Quest

A block-based coding education game built with Qt/C++ for CS 3505 at the University of Utah.

Players drag and drop code blocks to guide a character through puzzle levels. No typing is required: pick the right blocks, arrange them in order, and press Run to watch your code come to life. Each level introduces one programming concept, from basic method calls to loops and functions.

## Levels

| Level | Concept | Goal |
|-------|---------|------|
| 1 | Basic methods | Learn to use `move();` to send the character along route points |
| 2 | `if` statements | React to enemies: if you see one, wait |
| 3 | `for` loops | Simplify the code from the previous level by using a loop |
| 4 | Functions | Group several methods into a function and call it later |
| 5 | Level Check | Free-for-all: solve it with the skills you've learned, using the simplest code you can |

Levels unlock in order, and each level tracks your highest score.

## Key Features

* **Visual Block Workspace:** A drag-and-drop workspace for sequencing control flow, loops, and movement commands. Blocks can be reordered or deleted after they are placed.
* **Real-Time Execution & Highlighting:** The active block is highlighted as the interpreter runs your program.
* **Physics & Visual Feedback:** Box2D drives the confetti celebration and character/obstacle interaction.
* **Syntax & Logic Checking:** Custom structural checks (such as a `for` loop brace validator and scope rules) show an error message when a script is malformed.
* **Level Progression & Saves:** Level selection with locked levels, scores, win/lose pop-ups, and a save file that restores your progress automatically on startup.
* **Hints & Theming:** A Hint button with level-specific tips, plus a dark theme across the main window and code page.

## Tech Stack

* C++17 and Qt (Qt Widgets, Qt Designer `.ui` files, Qt resources)
* [Box2D](https://box2d.org/) physics engine (bundled in `Box2D/`)
* Progress saved as JSON in `save.txt`

## Project Structure

| File(s) | Responsibility |
|---------|----------------|
| `mainwindow.*` | Main window, level selection, code page, level setup, and run/submit logic |
| `workspace.*` | Drag-and-drop workspace widget (`dragEnterEvent` / `dropEvent`) |
| `codeBlock.*` | Individual draggable code blocks and their highlighting |
| `blockSpace.*` | Block storage area |
| `confetti.*` | Box2D-powered confetti effect |
| `savefile.*` | Save/load of unlocked levels and scores |
| `Box2D/` | Third-party physics library |

## Building and Running

1. Install [Qt](https://www.qt.io/download) with Qt Creator.
2. Open `EducationalApp.pro` in Qt Creator.
3. Configure the project with a desktop kit, then build and run.

## Team

Built by a team of five using Scrum-style sprints, daily stand-ups, and a GitHub project board.

| Member | Main contributions |
|--------|--------------------|
| Aspen Tobler | Drag-and-drop workspace and code blocks (add, reorder, delete), confetti effect, level design and puzzle logic, `for` loop checker, doc comments |
| Shaoze Yang | Level and map design, route points, player movement system, Box2D integration |
| Emily Wang | Project setup, task board, save file system, code documentation |
| Helen Zhang | UI: start page, code page, dark theme, button functionality, hint system |
| Xinmiao Li | Level selection UI, level unlocking, score display, level-complete pop-up, enemies |

## AI Usage

AI was used to help generate the `Workspace::dropEvent` drag-and-drop reordering logic (using the mouse position to find a block's new place in the workspace), to assist with task planning, and to polish level hint text.

University of Utah, CS 3505
