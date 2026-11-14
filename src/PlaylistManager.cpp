#include "PlaylistManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#if defined(__has_include)
    #if __has_include(<filesystem>)
        #include <filesystem>
        namespace fs = std::filesystem;
        #define HAS_STD_FILESYSTEM 1
    #elif __has_include(<experimental/filesystem>)
        #include <experimental/filesystem>
        namespace fs = std::experimental::filesystem;
        #define HAS_STD_FILESYSTEM 1
    #endif
#endif

#ifndef HAS_STD_FILESYSTEM
    #ifdef _WIN32
        #include <direct.h>
    #else
        #include <sys/stat.h>
        #include <sys/types.h>
    #endif
#endif

PlaylistManager::PlaylistManager(std::string filePath)
    : dataFilePath(std::move(filePath)), nextSongId(1) {
    ensureDataDirectoryExists();
    loadFromFile();
}

PlaylistManager::~PlaylistManager() {
    saveToFile();
}

void PlaylistManager::ensureDataDirectoryExists() const {
#ifdef HAS_STD_FILESYSTEM
    try {
        fs::path p(dataFilePath);
        if (p.has_parent_path()) {
            std::error_code ec;
            fs::create_directories(p.parent_path(), ec);
        }
    } catch (...) {}
#else
    size_t pos = dataFilePath.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string dir = dataFilePath.substr(0, pos);
        #ifdef _WIN32
            _mkdir(dir.c_str());
        #else
            mkdir(dir.c_str(), 0755);
        #endif
    }
#endif
}

std::string PlaylistManager::trim(const std::string& str) {
    size_t start = 0;
    // Strip UTF-8 BOM if present
    if (str.size() >= 3 &&
        static_cast<unsigned char>(str[0]) == 0xEF &&
        static_cast<unsigned char>(str[1]) == 0xBB &&
        static_cast<unsigned char>(str[2]) == 0xBF) {
        start = 3;
    }
    while (start < str.size() && (std::isspace(static_cast<unsigned char>(str[start])) || str[start] == '\0')) {
        start++;
    }
    if (start >= str.size()) return "";

    size_t end = str.size() - 1;
    while (end > start && (std::isspace(static_cast<unsigned char>(str[end])) || str[end] == '\0')) {
        end--;
    }
    return str.substr(start, end - start + 1);
}

void PlaylistManager::printHeader() const {
    std::cout << "\n==================================================\n";
    std::cout << "              MUSIC PLAYLIST MANAGER              \n";
    std::cout << "==================================================\n";
}

void PlaylistManager::printMenu() const {
    printHeader();
    std::cout << "  1. Add Song\n";
    std::cout << "  2. Delete Song\n";
    std::cout << "  3. Display Playlist (Forward)\n";
    std::cout << "  4. Display Playlist (Backward)\n";
    std::cout << "  5. Show Current Song\n";
    std::cout << "  6. Play Next Song\n";
    std::cout << "  7. Play Previous Song\n";
    std::cout << "  8. Search Song\n";
    std::cout << "  9. Shuffle Playlist\n";
    std::cout << "  10. Sort Playlist (A-Z)\n";
    std::cout << "  11. Count Total Songs\n";
    std::cout << "  0. Exit\n";
    std::cout << "==================================================\n";
}

void PlaylistManager::printSuccess(const std::string& message) {
    std::cout << "\n  [SUCCESS] " << message << "\n";
}

void PlaylistManager::printError(const std::string& message) {
    std::cout << "\n  [ERROR] " << message << "\n";
}

void PlaylistManager::printWarning(const std::string& message) {
    std::cout << "\n  [WARNING] " << message << "\n";
}

void PlaylistManager::printInfo(const std::string& message) {
    std::cout << "\n  [INFO] " << message << "\n";
}

void PlaylistManager::waitForEnter() {
    std::cout << "\n  Press [Enter] to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

int PlaylistManager::getValidatedInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 0; // EOF encountered
        }
        line = trim(line);
        if (line.empty()) {
            std::cout << "  [!] Input cannot be empty. Please enter a valid number.\n";
            continue;
        }

        bool isNumeric = true;
        size_t startIdx = 0;
        if (line[0] == '-' || line[0] == '+') {
            if (line.length() == 1) isNumeric = false;
            startIdx = 1;
        }
        for (size_t i = startIdx; i < line.length(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(line[i]))) {
                isNumeric = false;
                break;
            }
        }

        if (!isNumeric) {
            std::cout << "  [!] Invalid numeric format. Please enter digits only.\n";
            continue;
        }

        try {
            return std::stoi(line);
        } catch (const std::out_of_range&) {
            std::cout << "  [!] Number out of range. Please enter a smaller integer.\n";
        } catch (...) {
            std::cout << "  [!] Invalid input. Try again.\n";
        }
    }
}

std::string PlaylistManager::getValidatedString(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return "";
        }
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            std::cout << "  [!] Input cannot be empty or whitespace-only. Try again.\n";
            continue;
        }
        return trimmed;
    }
}

void PlaylistManager::loadFromFile() {
    std::ifstream inFile(dataFilePath);
    if (!inFile.is_open()) {
        // Missing file on fresh start is expected and handled gracefully
        nextSongId = 1;
        return;
    }

    playlist.clear();
    int maxId = 0;
    int loaded = 0;
    int malformed = 0;
    std::string line;

    while (std::getline(inFile, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;

        std::istringstream iss(trimmed);
        int id = 0;
        std::string title;
        std::string artist;

        if (iss >> id >> std::quoted(title) >> std::quoted(artist)) {
            playlist.addSong(id, title, artist);
            if (id > maxId) maxId = id;
            loaded++;
        } else {
            malformed++;
        }
    }

    inFile.close();

    if (playlist.getSize() > 0) {
        playlist.setCurrent(playlist.getHead());
    }
    nextSongId = maxId + 1;

    if (malformed > 0) {
        printWarning("Skipped " + std::to_string(malformed) + " malformed record(s) during data load.");
    }
}

bool PlaylistManager::saveToFile() const {
    std::ofstream outFile(dataFilePath, std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "  [!] Failed to open data file for writing: " << dataFilePath << "\n";
        return false;
    }

    SongNode* temp = playlist.getHead();
    while (temp != nullptr) {
        outFile << temp->id << " "
                << std::quoted(temp->title) << " "
                << std::quoted(temp->artist) << "\n";
        temp = temp->next;
    }

    outFile.close();
    return true;
}

void PlaylistManager::handleAddSong() {
    std::cout << "\n--- ADD NEW SONG ---\n";
    std::string title = getValidatedString("  Enter Song Title: ");
    if (title.empty()) return;
    std::string artist = getValidatedString("  Enter Artist Name: ");
    if (artist.empty()) return;

    int newId = nextSongId++;
    playlist.addSong(newId, title, artist);
    saveToFile();

    printSuccess("Song \"" + title + "\" by " + artist + " added successfully!");
    std::cout << "  Assigned Song ID: " << newId << "\n";
}

void PlaylistManager::handleDeleteSong() {
    if (playlist.isEmpty()) {
        printWarning("The playlist is empty. Nothing to delete.");
        return;
    }

    std::cout << "\n--- DELETE SONG ---\n";
    int id = getValidatedInt("  Enter Song ID to delete: ");

    SongNode* toDelete = playlist.findById(id);
    if (!toDelete) {
        printError("Song with ID " + std::to_string(id) + " was not found in the playlist.");
        return;
    }

    std::string deletedTitle = toDelete->title;
    bool success = playlist.removeSong(id);
    if (success) {
        saveToFile();
        printSuccess("Song ID " + std::to_string(id) + " (\"" + deletedTitle + "\") has been deleted.");
        if (playlist.isEmpty()) {
            printInfo("The playlist is now completely empty.");
        } else {
            SongNode* curr = playlist.getCurrentSong();
            if (curr) {
                std::cout << "  Active song updated to: \"" << curr->title << "\" by " << curr->artist << "\n";
            }
        }
    }
}

void PlaylistManager::handleDisplayForward() const {
    playlist.displayForward();
}

void PlaylistManager::handleDisplayBackward() const {
    playlist.displayBackward();
}

void PlaylistManager::handleShowCurrentSong() const {
    SongNode* curr = playlist.getCurrentSong();
    if (!curr) {
        printWarning("No song is currently active. The playlist is empty.");
        return;
    }

    std::cout << "\n==================================================\n";
    std::cout << "                 CURRENTLY PLAYING                \n";
    std::cout << "==================================================\n";
    std::cout << "  Song ID: " << curr->id << "\n";
    std::cout << "  Title  : " << curr->title << "\n";
    std::cout << "  Artist : " << curr->artist << "\n";
    std::cout << "==================================================\n";
}

void PlaylistManager::handlePlayNext() {
    if (playlist.isEmpty()) {
        printWarning("Cannot play next song: The playlist is empty.");
        return;
    }

    if (playlist.nextSong()) {
        SongNode* curr = playlist.getCurrentSong();
        printSuccess("Skipped to next song:");
        std::cout << "  ▶ Now Playing: \"" << curr->title << "\" by " << curr->artist << " (ID: " << curr->id << ")\n";
    } else {
        printWarning("Boundary reached: Already at the LAST song in the playlist.");
        SongNode* curr = playlist.getCurrentSong();
        if (curr) {
            std::cout << "  Current selection remains: \"" << curr->title << "\" by " << curr->artist << "\n";
        }
    }
}

void PlaylistManager::handlePlayPrevious() {
    if (playlist.isEmpty()) {
        printWarning("Cannot play previous song: The playlist is empty.");
        return;
    }

    if (playlist.previousSong()) {
        SongNode* curr = playlist.getCurrentSong();
        printSuccess("Rewound to previous song:");
        std::cout << "  ◀ Now Playing: \"" << curr->title << "\" by " << curr->artist << " (ID: " << curr->id << ")\n";
    } else {
        printWarning("Boundary reached: Already at the FIRST song in the playlist.");
        SongNode* curr = playlist.getCurrentSong();
        if (curr) {
            std::cout << "  Current selection remains: \"" << curr->title << "\" by " << curr->artist << "\n";
        }
    }
}

void PlaylistManager::handleSearchSong() const {
    if (playlist.isEmpty()) {
        printWarning("Cannot search: The playlist is empty.");
        return;
    }

    std::cout << "\n--- SEARCH SONG ---\n";
    std::cout << "  1. Search by Song ID\n";
    std::cout << "  2. Search by Song Title\n";
    std::cout << "  0. Back to Main Menu\n";
    int choice = getValidatedInt("  Select search method: ");

    if (choice == 1) {
        int id = getValidatedInt("  Enter Song ID to search: ");
        SongNode* match = playlist.findById(id);
        if (match) {
            printSuccess("Song found!");
            std::cout << "  ID    : " << match->id << "\n";
            std::cout << "  Title : " << match->title << "\n";
            std::cout << "  Artist: " << match->artist << "\n";
            if (match == playlist.getCurrentSong()) {
                std::cout << "  Status: [CURRENTLY PLAYING]\n";
            }
        } else {
            printError("No song found with ID " + std::to_string(id) + ".");
        }
    } else if (choice == 2) {
        std::string title = getValidatedString("  Enter Song Title to search: ");
        SongNode* match = playlist.findByTitle(title);
        if (match) {
            printSuccess("Song found!");
            std::cout << "  ID    : " << match->id << "\n";
            std::cout << "  Title : " << match->title << "\n";
            std::cout << "  Artist: " << match->artist << "\n";
            if (match == playlist.getCurrentSong()) {
                std::cout << "  Status: [CURRENTLY PLAYING]\n";
            }
        } else {
            printError("No song found matching title \"" + title + "\" (case-insensitive).");
        }
    } else {
        std::cout << "  Returning to main menu.\n";
    }
}

void PlaylistManager::handleShuffle() {
    if (playlist.isEmpty()) {
        printWarning("Cannot shuffle: The playlist is empty.");
        return;
    }

    SongNode* randomSong = playlist.getRandomSong();
    if (randomSong) {
        printSuccess("Shuffled! Selected a random song from the playlist:");
        std::cout << "  🔀 Now Playing: \"" << randomSong->title << "\" by " << randomSong->artist << " (ID: " << randomSong->id << ")\n";
        std::cout << "  Note: Playlist structure and order remain intact.\n";
    }
}

void PlaylistManager::handleSort() {
    if (playlist.isEmpty()) {
        printWarning("Cannot sort: The playlist is empty.");
        return;
    }
    if (playlist.getSize() == 1) {
        printInfo("Playlist contains only 1 song; already sorted.");
        return;
    }

    playlist.sortByTitle();
    saveToFile();

    printSuccess("Playlist sorted alphabetically by Song Title (A-Z)!");
    SongNode* curr = playlist.getCurrentSong();
    if (curr) {
        std::cout << "  Current song selection preserved: \"" << curr->title << "\" by " << curr->artist << "\n";
    }
}

void PlaylistManager::handleCountSongs() const {
    int total = playlist.getSize();
    std::cout << "\n==================================================\n";
    std::cout << "  Total songs in playlist: " << total << "\n";
    std::cout << "==================================================\n";
}

void PlaylistManager::run() {
    bool running = true;
    while (running) {
        printMenu();
        int choice = getValidatedInt("  Enter your choice: ");

        switch (choice) {
            case 1:
                handleAddSong();
                waitForEnter();
                break;
            case 2:
                handleDeleteSong();
                waitForEnter();
                break;
            case 3:
                handleDisplayForward();
                waitForEnter();
                break;
            case 4:
                handleDisplayBackward();
                waitForEnter();
                break;
            case 5:
                handleShowCurrentSong();
                waitForEnter();
                break;
            case 6:
                handlePlayNext();
                waitForEnter();
                break;
            case 7:
                handlePlayPrevious();
                waitForEnter();
                break;
            case 8:
                handleSearchSong();
                waitForEnter();
                break;
            case 9:
                handleShuffle();
                waitForEnter();
                break;
            case 10:
                handleSort();
                waitForEnter();
                break;
            case 11:
                handleCountSongs();
                waitForEnter();
                break;
            case 0:
                printInfo("Saving playlist and exiting Music Playlist Manager. Goodbye!");
                saveToFile();
                running = false;
                break;
            default:
                printError("Invalid choice (" + std::to_string(choice) + "). Please select from 0 to 11.");
                waitForEnter();
                break;
        }
    }
}

const DoublyLinkedList& PlaylistManager::getPlaylist() const {
    return playlist;
}

DoublyLinkedList& PlaylistManager::getPlaylist() {
    return playlist;
}

void PlaylistManager::reloadData() {
    loadFromFile();
}

bool PlaylistManager::saveData() const {
    return saveToFile();
}
