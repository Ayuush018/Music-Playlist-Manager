# Music Playlist Manager

A modular, high-performance **Music Playlist Manager** written in **C++17**, engineered for the **Advanced Data Structures and Algorithms (ADSA)** curriculum. The core of this project is a custom **Doubly Linked List** built completely from scratch without reliance on `std::list`, `std::vector`, or third-party container libraries.

The project features two distinct user interfaces built on top of the exact same data structure backend:
1. **Interactive Console CLI (`MusicPlaylistManagerCLI`):** A terminal-based menu-driven management system.
2. **Modern Desktop GUI (`MusicPlaylistManagerGUI`):** A Spotify-inspired dark desktop application built with **Qt 6 Widgets**, providing real-time visual feedback, responsive layouts, search filtering, inline deletion, modal dialogs, and playback controls.

> **Note:** This project is a playlist metadata and queue management system, not an audio decoder or streaming client. It does not play audio files.

---

## Project Report

The complete academic project report for this Advanced Data Structures (ADS) course project is available in PDF format:

📄 **[View Project Report (PDF)](docs/Music_Playlist_Manager_Project_Report_Ayush_Chauhan.pdf)**

The report provides comprehensive academic documentation covering:
- **System Architecture & Design:** Layered three-tier architecture decoupling the presentation frontends from the data structure core.
- **Custom Doubly Linked List:** Manual heap memory allocation, raw pointer manipulation (`prev`, `next`, `head`, `tail`, `current`), boundary safety, and selection cursor management.
- **Algorithms & Complexity Analysis:** $O(1)$ tail appends, $O(N)$ deletion with pointer relinking, true in-place alphabetical sorting without auxiliary buffers, uniform random shuffle using `<random>` (`std::mt19937`), and comprehensive Big-O justifications.
- **Modern Qt 6 Desktop GUI:** Spotify-inspired dark aesthetic (QSS), responsive layout, direct table binding without duplicate containers, modal dialogs, and real-time search filtering.
- **Implementation Details:** Verified C++17 and Qt 6 code listings for `SongNode`, `DoublyLinkedList`, `PlaylistManager`, `MainWindow`, and `CMakeLists.txt`.
- **Testing & Verification:** Headless automated test suite (`tests/verify_gui_and_backend.cpp`), 10-point test case matrix, defensive boundary checks, and empirical results.

---

## Table of Contents
- [Project Report](#project-report)
1. [Project Overview](#project-overview)
2. [Academic Objectives](#academic-objectives)
3. [Key Features](#key-features)
4. [Technologies Used](#technologies-used)
5. [Data Structure Architecture](#data-structure-architecture)
6. [Doubly Linked List Mechanics](#doubly-linked-list-mechanics)
7. [Qt 6 GUI Desktop Interface](#qt-6-gui-desktop-interface)
8. [Project Structure](#project-structure)
9. [Installation & Requirements](#installation--requirements)
   - [Compiler Installation](#compiler-installation)
   - [Qt 6 Setup Guide](#qt-6-setup-guide)
10. [Build and Execution](#build-and-execution)
   - [Building with CMake (CLI & GUI)](#building-with-cmake-cli--gui)
   - [Direct CLI Compilation with g++](#direct-cli-compilation-with-g)
11. [Application Usage & Workflow](#application-usage--workflow)
12. [File Persistence Mechanism](#file-persistence-mechanism)
13. [Time and Space Complexity Analysis](#time-and-space-complexity-analysis)
14. [Limitations](#limitations)
15. [Future Roadmap](#future-roadmap)
16. [Example Outputs](#example-outputs)
17. [Author & Contributors](#author--contributors)
18. [License](#license)

---

## 1. Project Overview

Managing a music playlist requires flexible sequence traversal, seamless insertion and removal at endpoints or interior positions, bidirectional navigation (previous / next track), and state persistence. While simple array structures suffer from $O(N)$ data shifting during interior deletions and single linked lists lack backward traversal capabilities, a **Doubly Linked List** provides the optimal operational profile for dynamic playlist tracking.

This project implements:
- Direct raw pointer manipulation (`prev`, `next`, `head`, `tail`, `current`).
- Manual heap memory management using `new` and `delete`.
- Strict destructor deallocation preventing memory leaks.
- Copy prevention (deleted copy constructor / assignment) to avoid dangling pointer shallow copies.
- File serialization preserving song data, titles with spaces, and special symbols across sessions.
- In-place alphabetical sorting by pointer rewiring without converting into auxiliary arrays.
- Dual frontend architecture (Console CLI & Qt 6 GUI) sharing a single source of truth.

---

## 2. Academic Objectives

- **Pointer Mastery:** Solidify understanding of two-way pointer links (`prev` $\leftrightarrow$ `next`) and edge-case pointer updating (endpoints, single-node lists, empty lists).
- **Dynamic Memory Safety:** Enforce proper lifecycle management where each allocated `SongNode` is explicitly tracked and deallocated.
- **Algorithm Design:** Implement in-place sorting and bidirectional traversal over node pointers.
- **Single Source of Truth:** Demonstrate how a modern graphical user interface can bind directly to an underlying custom data structure without duplicating storage.
- **Defensive Engineering:** Implement robust input validation, fault-tolerant deserialization, and cross-platform compilation.

---

## 3. Key Features

- **Add Song:** Creates a new track node, auto-increments unique IDs, appends to the playlist tail, and persists to disk.
- **Delete Song:** Locates by ID, cleanly stitches neighboring pointers, updates `head`/`tail`/`current`, deallocates memory, and saves changes.
- **Bidirectional Traversal:** View playlist ordered forward (`head` $\to$ `tail`) or backward (`tail` $\to$ `head`), displaying full track metadata and marking the currently playing track.
- **Track Navigation:** Advance to next song or rewind to previous song, respecting sequence boundary guards.
- **Active Track Inspection:** Displays complete details for the active `current` song (`NOW SELECTED`).
- **Case-Insensitive Search:** Query songs by unique ID or by song title (with automatic trimming and case folding). In the GUI, search dynamically filters table rows.
- **True In-Place Alphabetical Sorting:** Sorts tracks (A-Z) by rewiring node pointers directly on the doubly linked list.
- **Shuffle Selection:** Uses `<random>` (`std::mt19937`) to pick a uniformly distributed random track and updates the `current` cursor without mutating playlist sequence order.
- **Double-Click Row Selection (GUI):** Double-clicking any row in the GUI table immediately sets that track as active in the underlying Doubly Linked List.
- **Crash-Resilient Persistence:** Loads previously saved playlists on boot, skips corrupted lines gracefully, and guarantees unique ID resumption.

---

## 4. Technologies Used

- **Programming Language:** C++17
- **Primary Data Structure:** Custom Doubly Linked List (`SongNode`)
- **Desktop GUI Framework:** Qt 6 (Qt Widgets, QSS Styling)
- **Memory Management:** Raw dynamic pointers (`new`, `delete`), RAII lifecycle
- **I/O & Persistence:** Standard streams (`<iostream>`, `<fstream>`, `<sstream>`, `<iomanip>` with `std::quoted`)
- **Randomization:** `<random>` (`std::random_device`, `std::mt19937`, `std::uniform_int_distribution`)
- **Build System:** CMake 3.16+
- **Platforms Supported:** Windows 10/11, Linux (Ubuntu, Debian, Fedora, Arch), macOS

---

## 5. Data Structure Architecture

### The `SongNode` Component

Each element in the playlist is represented by an allocated `SongNode`:

```cpp
struct SongNode {
    int id;               // Unique auto-incremented song identifier
    std::string title;    // Song title (supports spaces and symbols)
    std::string artist;   // Artist / composer name
    SongNode* prev;       // Pointer to preceding node (or nullptr if head)
    SongNode* next;       // Pointer to succeeding node (or nullptr if tail)
};
```

### The `DoublyLinkedList` Class

Maintains list state and encapsulation:
- `SongNode* head`: Points to the first track in the sequence.
- `SongNode* tail`: Points to the final track in the sequence.
- `SongNode* current`: Points to the currently active / selected track.
- `int size`: Tracks the count of allocated nodes.

---

## 6. Doubly Linked List Mechanics

### Pointer Structure Diagram

```text
       nullptr
          ^
          | (prev)
     +---------+      next      +---------+      next      +---------+
     | Song 1  | -------------> | Song 2  | -------------> | Song 3  | -----> nullptr
     | ID: 1   | <------------- | ID: 2   | <------------- | ID: 3   |
     +---------+      prev      +---------+      prev      +---------+
          ^                          ^                          ^
          |                          |                          |
        [HEAD]                   [CURRENT]                   [TAIL]
```

### Pointer Management Invariants

1. **Empty List:** `head == nullptr`, `tail == nullptr`, `current == nullptr`, `size == 0`.
2. **Single Element:** `head == tail == current`, `head->prev == nullptr`, `head->next == nullptr`, `size == 1`.
3. **Head Boundary:** `head->prev` is always guaranteed to be `nullptr`.
4. **Tail Boundary:** `tail->next` is always guaranteed to be `nullptr`.
5. **Bidirectional Symmetry:** For any interior node `N`, `N->next->prev == N` and `N->prev->next == N`.
6. **Deletion Fallback:** When `current` is removed, selection advances to `target->next`; if at the tail, it retreats to `target->prev`; if the list is emptied, it resets to `nullptr`.

---

## 7. Qt 6 GUI Desktop Interface

The desktop GUI (`MusicPlaylistManagerGUI`) provides a Spotify-inspired dark aesthetic while binding strictly to the `PlaylistManager` and `DoublyLinkedList` backend:

```text
+-----------------------------------------------------------------------------------------------+
|  ♫ MUSIC MANAGER   |  My Music Collection                                                     |
|                    |  Powered by Custom Doubly Linked List with In-Place Pointer Sorting      |
|  🏠 Home           |                                                                          |
|  🎵 My Playlist    |  [ 🔍 Search by title or artist... ]  [+ Add Song]  [🔀 Shuffle]  [🔤 Sort]|
|  📑 All Songs      +--------------------------------------------------------------------------+
|  🔍 Search Library |  ID   | TITLE                    | ARTIST          | ACTION              |
|                    |  1    | ▶ Bohemian Rhapsody     | Queen           | [🗑 Delete]         |
|  PLAYLIST INFO     |  2    | Hotel California         | Eagles          | [🗑 Delete]         |
|  Total Songs: 5    |  3    | Imagine                  | John Lennon     | [🗑 Delete]         |
|  File: playlist.txt|  4    | Billie Jean              | Michael Jackson | [🗑 Delete]         |
|  Engine: DLL (ADSA)|  5    | Sweet Child O' Mine      | Guns N' Roses   | [🗑 Delete]         |
+--------------------+--------------------------------------------------------------------------+
| NOW SELECTED       |                                                                          |
| Bohemian Rhapsody  |                     [ ⏮ Prev ]    [ 🔀 Shuffle ]    [ Next ⏭ ]        |
| Queen              |                                                   Selected ID: 1 • In RAM|
+-----------------------------------------------------------------------------------------------+
```

### GUI Design Highlights
- **Palette:** Dark charcoal (`#121212`), surface cards (`#181818`), input fields (`#242424`), and Spotify green accent (`#1DB954`).
- **Responsive Three-Section Layout:** Fixed 220px left sidebar, flexible central table area, and 90px fixed bottom panel.
- **Direct Backend Refresh:** The GUI table never maintains an independent copy of playlist nodes. It iterates from `head` through `->next` on demand.
- **Real-Time Search Filter:** Typing in the search bar dynamically filters table visibility without changing the backend selection.
- **Double-Click Interaction:** Double-clicking any song selects it as the active `current` pointer in the linked list.
- **Safe Dialogs:** Dedicated `AddSongDialog` validates input, and deletion prompts a `QMessageBox` confirmation.

---

## 8. Project Structure

```text
Music-Playlist-Manager/
├── include/
│   ├── Song.h               # SongNode declaration and display interfaces
│   ├── DoublyLinkedList.h   # DoublyLinkedList class declaration and pointer methods
│   ├── PlaylistManager.h    # UI coordinator, validation, and file persistence
│   └── MainWindow.h         # Qt 6 MainWindow and AddSongDialog declarations
├── src/
│   ├── main.cpp             # Legacy CLI entry point
│   ├── main_cli.cpp         # Dedicated CLI application entry point
│   ├── main_gui.cpp         # Qt 6 GUI application entry point
│   ├── Song.cpp             # SongNode implementation
│   ├── DoublyLinkedList.cpp # Pointer-level linked list operations
│   ├── PlaylistManager.cpp  # Menu management, input handling, serialization
│   └── MainWindow.cpp       # Qt 6 GUI implementation and Spotify dark styling
├── data/
│   └── playlist.txt         # Persistent text storage for playlist tracks
├── CMakeLists.txt           # Multi-target build configuration (CLI + Qt 6 GUI)
├── .gitignore               # Excludes binaries, object files, Qt MOC, and IDE files
├── LICENSE                  # MIT License
└── README.md                # Comprehensive documentation
```

---

## 9. Installation & Requirements

### Compiler Installation
- **Windows:** MinGW-w64 (GCC 7+) or MSVC (Visual Studio 2019/2022).
- **Linux:** `sudo apt install build-essential cmake`
- **macOS:** `xcode-select --install`

### Qt 6 Setup Guide
The GUI target requires **Qt 6 (Widgets module)**:
- **Windows (Configured Environment):**
  - **Qt Version:** Qt 6.12.0 MinGW 64-bit kit installed at `C:\Qt\6.12.0\mingw_64`.
  - **Compiler:** MinGW GCC 13.1.0 64-bit at `C:\Qt\Tools\mingw1310_64\bin`.
  - **Build Tool:** Ninja at `C:\Qt\Tools\Ninja\ninja.exe` and CMake at `C:\Qt\Tools\CMake_64\bin\cmake.exe`.
- **Linux (Ubuntu / Debian):**
  ```bash
  sudo apt install qt6-base-dev libqt6widgets6 ninja-build
  ```
- **macOS (Homebrew):**
  ```bash
  brew install qt@6 ninja
  ```

---

## 10. Build and Execution

### Building with CMake and Qt 6.12 (Verified on Windows)

CMake is configured to build both executables:
- `MusicPlaylistManagerCLI` (Standalone Console Application)
- `MusicPlaylistManagerGUI` (Modern Qt 6 Desktop Application)

**1. Configure with Qt 6.12 MinGW 64-bit kit:**
```powershell
$env:Path = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;C:\Qt\6.12.0\mingw_64\bin;C:\Qt\Tools\CMake_64\bin;" + $env:Path

cmake -S . -B build -G "Ninja" `
  -DCMAKE_PREFIX_PATH="C:/Qt/6.12.0/mingw_64" `
  -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" `
  -DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/Ninja/ninja.exe"
```

**2. Compile all targets:**
```powershell
cmake --build build
```

**3. Deploy runtime dependencies for GUI (First-time deployment):**
```powershell
& "C:\Qt\6.12.0\mingw_64\bin\windeployqt.exe" --no-translations "build/MusicPlaylistManagerGUI.exe"
```

**4. Run Applications:**
```powershell
# Run the Desktop GUI:
.\build\MusicPlaylistManagerGUI.exe

# Run the Console CLI:
.\build\MusicPlaylistManagerCLI.exe
```

---

### Direct CLI Compilation with g++

The console application can also be compiled directly with any standard C++17 compiler without CMake or Qt:

**Windows:**
```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Song.cpp src/DoublyLinkedList.cpp src/PlaylistManager.cpp -o MusicPlaylistManagerCLI.exe
.\MusicPlaylistManagerCLI.exe
```

**Linux / macOS:**
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Song.cpp src/DoublyLinkedList.cpp src/PlaylistManager.cpp -o MusicPlaylistManagerCLI
./MusicPlaylistManagerCLI
```

---

## 11. Application Usage & Workflow

### Console Menu Interface
```text
==================================================
              MUSIC PLAYLIST MANAGER              
==================================================
  1. Add Song
  2. Delete Song
  3. Display Playlist (Forward)
  4. Display Playlist (Backward)
  5. Show Current Song
  6. Play Next Song
  7. Play Previous Song
  8. Search Song
  9. Shuffle Playlist
  10. Sort Playlist (A-Z)
  11. Count Total Songs
  0. Exit
==================================================
  Enter your choice:
```

### Desktop GUI Workflow
1. **Launch:** Run `MusicPlaylistManagerGUI`. Previously saved songs in `data/playlist.txt` load automatically.
2. **Add:** Click `＋ Add Song` on the toolbar. Enter title and artist. Click `Add Song`.
3. **Select:** Double-click any row to set it as active (`NOW SELECTED`).
4. **Navigate:** Click `⏮ Prev` or `Next ⏭` in the bottom bar to move sequentially across the playlist.
5. **Shuffle:** Click `🔀 Shuffle` to jump to an unbiased random track without changing song positions.
6. **Sort:** Click `🔤 Sort (A-Z)` to sort titles alphabetically in-place via pointer rewiring.
7. **Search:** Type in the search bar to filter matching songs instantly.

---

## 12. File Persistence Mechanism

The playlist automatically synchronizes with `data/playlist.txt`.

### Storage Format
Records are serialized line-by-line using `std::quoted` from `<iomanip>`:
```text
1 "Bohemian Rhapsody" "Queen"
2 "Hotel California" "Eagles"
3 "Imagine" "John Lennon"
```

### Key Persistence Highlights
- **Escaping Special Characters:** `std::quoted` safely encapsulates spaces, punctuation, quotes, and commas without field corruption.
- **Automatic ID Recovery:** When loading songs, the manager scans existing IDs and sets `nextSongId = max(existing_id) + 1`. This prevents ID collision when adding new tracks after a program restart.
- **Fault-Tolerant Parsing:** If a file line contains corrupted or incomplete tokens, the manager logs a warning, skips that single line, and continues parsing the remaining valid songs.
- **Auto Directory Creation:** If the `data/` directory does not exist, the application creates it automatically before writing.

---

## 13. Time and Space Complexity Analysis

| Operation | Time Complexity (Average) | Time Complexity (Worst Case) | Space Complexity (Auxiliary) | Justification |
| :--- | :---: | :---: | :---: | :--- |
| **Add Song (Append)** | $O(1)$ | $O(1)$ | $O(1)$ | Tail pointer enables direct constant-time node insertion. |
| **Delete Song (by ID)** | $O(N)$ | $O(N)$ | $O(1)$ | Requires sequential traversal to locate node; unlinking is $O(1)$. |
| **Search by ID** | $O(N)$ | $O(N)$ | $O(1)$ | Linear scan from head to matching ID node. |
| **Search by Title** | $O(N)$ | $O(N)$ | $O(1)$ | Linear scan with case-insensitive string comparison. |
| **Display Forward** | $O(N)$ | $O(N)$ | $O(1)$ | Visits all $N$ nodes sequentially from head to tail. |
| **Display Backward** | $O(N)$ | $O(N)$ | $O(1)$ | Visits all $N$ nodes sequentially from tail to head via `prev`. |
| **Next Song** | $O(1)$ | $O(1)$ | $O(1)$ | Direct pointer step to `current->next`. |
| **Previous Song** | $O(1)$ | $O(1)$ | $O(1)$ | Direct pointer step to `current->prev`. |
| **Get Current Song** | $O(1)$ | $O(1)$ | $O(1)$ | Returns stored pointer `current`. |
| **Song Count** | $O(1)$ | $O(1)$ | $O(1)$ | Returns stored integer `size`. |
| **Shuffle** | $O(N)$ | $O(N)$ | $O(1)$ | Uniform index generated in $O(1)$, followed by traversal to index. |
| **Alphabetical Sort** | $O(N^2)$ | $O(N^2)$ | $O(1)$ | In-place adjacent node pointer swapping without auxiliary arrays. |
| **File Load** | $O(N)$ | $O(N)$ | $O(1)$ | Sequential file read and append of $N$ records. |
| **File Save** | $O(N)$ | $O(N)$ | $O(1)$ | Sequential write traversal from head to tail. |

### Why a Doubly Linked List?
1. **True Bidirectional Navigation:** Unlike a singly linked list where moving backwards requires $O(N)$ re-traversal from the head, each node has a `prev` pointer providing immediate $O(1)$ previous track access.
2. **Instant Endpoint Operations:** Having explicit `head` and `tail` pointers permits $O(1)$ appends and removals at both ends of the playlist.
3. **In-Place Restructuring:** Re-ordering and node deletions only rewire pointers; no array reallocation or mass memory shifting occurs.

---

## 14. Limitations

- **Sequential Access for Searching:** Since nodes are linked via pointers rather than contiguous memory, binary search cannot be applied; lookup is bounded by $O(N)$.
- **In-Memory Queue:** Playlist updates operate in RAM before being flushed to disk.

---

## 15. Future Roadmap

- [ ] Add circular playlist looping mode (`tail->next = head` and `head->prev = tail`).
- [ ] Implement Merge Sort on the doubly linked list to achieve $O(N \log N)$ sorting time.
- [ ] Support drag-and-drop song reordering in the GUI table by rewiring linked-list node pointers.
- [ ] Support multiple distinct playlists through a playlist catalog manager.

---

## 16. Example Outputs

### CLI Output
```text
========================================================================================
                               PLAYLIST (FORWARD: HEAD -> TAIL)                         
========================================================================================
  Status     ID    Title                        Artist
----------------------------------------------------------------------------------------
[CURRENT] ▶ ID: 1    | Title: Bohemian Rhapsody            | Artist: Queen                 
            ID: 2    | Title: Hotel California             | Artist: Eagles                
            ID: 3    | Title: Imagine                      | Artist: John Lennon           
            ID: 4    | Title: Billie Jean                  | Artist: Michael Jackson       
            ID: 5    | Title: Sweet Child O' Mine          | Artist: Guns N' Roses         
========================================================================================
  Total Songs: 5
```

---

## 17. Author & Contributors

- **Author / Developer:** OsmosisBinLaden ([@Ayuush018](https://github.com/Ayuush018))
- **Course:** Advanced Data Structures and Algorithms (ADSA)
- **Repository:** [https://github.com/Ayuush018/Music-Playlist-Manager](https://github.com/Ayuush018/Music-Playlist-Manager)

---

## 18. License

This project is licensed under the [MIT License](LICENSE) - open for educational and portfolio demonstration use.
