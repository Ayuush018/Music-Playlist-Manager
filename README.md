# Music Playlist Manager

A modular, high-performance, console-based **Music Playlist Manager** written in **C++17**, engineered for the **Advanced Data Structures and Algorithms (ADSA)** curriculum. The core of this project is a custom **Doubly Linked List** built completely from scratch without reliance on `std::list`, `std::vector`, or third-party container libraries.

> **Note:** This project is a playlist metadata and queue management system, not an audio decoder or streaming client. It does not play audio files.

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Academic Objectives](#academic-objectives)
3. [Key Features](#key-features)
4. [Technologies Used](#technologies-used)
5. [Data Structure Architecture](#data-structure-architecture)
6. [Doubly Linked List Mechanics](#doubly-linked-list-mechanics)
7. [Project Structure](#project-structure)
8. [Compiler Installation Guide](#compiler-installation-guide)
9. [Build and Execution](#build-and-execution)
   - [Method 1: Direct Compilation with g++](#method-1-direct-compilation-with-g)
   - [Method 2: Building with CMake](#method-2-building-with-cmake)
10. [Application Usage & Workflow](#application-usage--workflow)
11. [File Persistence Mechanism](#file-persistence-mechanism)
12. [Time and Space Complexity Analysis](#time-and-space-complexity-analysis)
13. [Limitations](#limitations)
14. [Future Roadmap](#future-roadmap)
15. [Example Terminal Output](#example-terminal-output)
16. [Author & Contributors](#author--contributors)
17. [License](#license)

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

---

## 2. Academic Objectives

- **Pointer Mastery:** Solidify understanding of two-way pointer links (`prev` $\leftrightarrow$ `next`) and edge-case pointer updating (endpoints, single-node lists, empty lists).
- **Dynamic Memory Safety:** Enforce proper lifecycle management where each allocated `SongNode` is explicitly tracked and deallocated.
- **Algorithm Design:** Implement in-place sorting and bidirectional traversal over node pointers.
- **Robust Engineering:** Practice defensive input validation, cross-platform file stream management, and clean modular class design.

---

## 3. Key Features

- **Add Song:** Creates a new track node, auto-increments unique IDs, appends to the playlist tail, and persists to disk.
- **Delete Song:** Locates by ID, cleanly stitches neighboring pointers, updates `head`/`tail`/`current`, deallocates memory, and saves changes.
- **Bidirectional Traversal:** View playlist ordered forward (`head` $\to$ `tail`) or backward (`tail` $\to$ `head`), displaying full track metadata and marking the currently playing track.
- **Track Navigation:** Advance to next song or rewind to previous song, respecting sequence boundary guards.
- **Active Track Inspection:** Displays complete details for the active `current` song.
- **Case-Insensitive Search:** Query songs by unique ID or by song title (with automatic trimming and case folding).
- **True In-Place Alphabetical Sorting:** Sorts tracks (A-Z) by rewiring node pointers directly on the doubly linked list.
- **Shuffle Play:** Uses `<random>` (`std::mt19937`) to pick a uniformly distributed random track and updates the `current` cursor without mutating playlist sequence order.
- **Crash-Resilient Persistence:** Loads previously saved playlists on boot, skips corrupted lines gracefully, and guarantees unique ID resumption.

---

## 4. Technologies Used

- **Language:** C++17
- **Primary Data Structure:** Custom Doubly Linked List (`SongNode`)
- **Memory Management:** Raw dynamic pointers (`new`, `delete`), RAII destructor
- **I/O & Persistence:** Standard streams (`<iostream>`, `<fstream>`, `<sstream>`, `<iomanip>` with `std::quoted`)
- **Randomization:** `<random>` (`std::random_device`, `std::mt19937`, `std::uniform_int_distribution`)
- **Build Systems:** CMake (3.12+) and direct `g++` compilation
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
- `SongNode* current`: Points to the currently active / playing track.
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

## 7. Project Structure

```text
Music-Playlist-Manager/
├── include/
│   ├── Song.h               # SongNode declaration and display interfaces
│   ├── DoublyLinkedList.h   # DoublyLinkedList class declaration
│   └── PlaylistManager.h    # UI coordinator, validation, and file I/O
├── src/
│   ├── main.cpp             # Application entry point
│   ├── Song.cpp             # SongNode implementation
│   ├── DoublyLinkedList.cpp # Pointer-level linked list operations
│   └── PlaylistManager.cpp  # Menu management, input handling, serialization
├── data/
│   └── playlist.txt         # Persistent text storage for playlist tracks
├── CMakeLists.txt           # Standard CMake build configuration
├── .gitignore               # Ignored build artifacts and temp files
├── LICENSE                  # Open-source license file
└── README.md                # Comprehensive documentation
```

---

## 8. Compiler Installation Guide

### Windows
- **MinGW-w64 (GCC):** Install via [MSYS2](https://www.msys2.org/) or [WinLibs](https://winlibs.com/). Add `bin` folder to system `PATH`.
- Verify in PowerShell:
  ```powershell
  g++ --version
  ```

### Linux (Ubuntu / Debian)
```bash
sudo apt update
sudo apt install build-essential cmake
g++ --version
```

### macOS
```bash
xcode-select --install
# Optionally install cmake via Homebrew:
brew install cmake
clang++ --version
```

---

## 9. Build and Execution

### Method 1: Direct Compilation with g++

Navigate to the project root directory:

**Windows (PowerShell / Command Prompt):**
```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Song.cpp src/DoublyLinkedList.cpp src/PlaylistManager.cpp -o MusicPlaylistManager.exe
.\MusicPlaylistManager.exe
```

**Linux / macOS (Terminal):**
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Song.cpp src/DoublyLinkedList.cpp src/PlaylistManager.cpp -o MusicPlaylistManager
./MusicPlaylistManager
```

---

### Method 2: Building with CMake

```bash
# 1. Create and enter build directory
mkdir build
cd build

# 2. Generate build files
cmake ..

# 3. Build executable
cmake --build .

# 4. Run application
# On Windows:
.\MusicPlaylistManager.exe
# On Linux / macOS:
./MusicPlaylistManager
```

---

## 10. Application Usage & Workflow

Upon launching the executable, the user is presented with the interactive main menu:

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

### Input Safety & Resilience
- **Non-numeric protection:** Entering letters or symbols when a numeric choice or ID is requested prompts a descriptive error without crashing or entering infinite loops.
- **Empty input rejection:** Empty strings or whitespace-only inputs for song titles and artists are rejected.
- **Boundary feedback:** Attempting to play `next` on the last song or `previous` on the first song alerts the user while keeping the current track selected.

---

## 11. File Persistence Mechanism

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

## 12. Time and Space Complexity Analysis

The table below describes the actual algorithmic complexities of all major operations:

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

## 13. Limitations

- **Sequential Access for Searching:** Since nodes are linked via pointers rather than contiguous memory, binary search cannot be applied; lookup is bounded by $O(N)$.
- **Terminal Display Truncation:** Very long titles ($> 28$ characters) or artists ($> 22$ characters) are displayed with an ellipsis (`...`) in tabular listings to maintain formatting alignment (full titles remain preserved in memory and storage).
- **In-Memory Queue:** Playlist updates operate in RAM before being flushed to disk.

---

## 14. Future Roadmap

- [ ] Add circular playlist looping mode (where `tail->next = head` and `head->prev = tail`).
- [ ] Implement Merge Sort on the doubly linked list to achieve $O(N \log N)$ sorting time.
- [ ] Support multiple distinct playlists through a playlist catalog manager.
- [ ] Implement song duration tracking and total playlist playtime calculations.

---

## 15. Example Terminal Output

### Displaying Playlist
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

### Playing Next Track
```text
  [SUCCESS] Skipped to next song:
  ▶ Now Playing: "Hotel California" by Eagles (ID: 2)
```

---

## 16. Author & Contributors

- **Course:** Advanced Data Structures and Algorithms (ADSA)
- **Developer / Student:** `[Your Name / Placeholder]`
- **Repository:** `[Your GitHub Repository Link]`

---

## 17. License

This project is licensed under the [MIT License](LICENSE) - open for educational and portfolio demonstration use.
