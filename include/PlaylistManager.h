#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include "DoublyLinkedList.h"
#include <string>

/**
 * @brief Coordinates application-level operations, terminal UI,
 * input validation, and file persistence.
 */
class PlaylistManager {
private:
    DoublyLinkedList playlist;
    std::string dataFilePath;
    int nextSongId;

    // File handling
    void loadFromFile();
    bool saveToFile() const;
    void ensureDataDirectoryExists() const;

    // Terminal UI Helpers
    void printHeader() const;
    void printMenu() const;
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);

    // Reusable Input Validation Helpers
    static std::string trim(const std::string& str);
    static int getValidatedInt(const std::string& prompt);
    static std::string getValidatedString(const std::string& prompt);
    static void waitForEnter();

    // Menu Action Handlers
    void handleAddSong();
    void handleDeleteSong();
    void handleDisplayForward() const;
    void handleDisplayBackward() const;
    void handleShowCurrentSong() const;
    void handlePlayNext();
    void handlePlayPrevious();
    void handleSearchSong() const;
    void handleShuffle();
    void handleSort();
    void handleCountSongs() const;

public:
    /**
     * @brief Constructor for PlaylistManager.
     * @param filePath Path to the persistence data file (default: "data/playlist.txt").
     */
    explicit PlaylistManager(std::string filePath = "data/playlist.txt");

    /**
     * @brief Destructor. Ensures playlist is cleanly saved before exit.
     */
    ~PlaylistManager();

    /**
     * @brief Starts the main interactive terminal application loop.
     */
    void run();

    /**
     * @brief Exposes internal list for testing and verification purposes.
     */
    const DoublyLinkedList& getPlaylist() const;
    DoublyLinkedList& getPlaylist();

    /**
     * @brief Public access to file load/save for scripted testing.
     */
    void reloadData();
    bool saveData() const;

    // --- Programmatic API for GUI & external coordinators ---
    /**
     * @brief Programmatically adds a song with auto-increment ID and persists to file.
     * @return Assigned unique ID on success, or -1 on invalid input.
     */
    int addSong(const std::string& title, const std::string& artist);

    /**
     * @brief Programmatically deletes a song by ID, updates selection, and persists.
     * @return true if deleted, false if not found.
     */
    bool deleteSong(int id);

    /**
     * @brief Programmatically sorts the playlist in-place and persists.
     */
    void sortPlaylist();

    /**
     * @brief Programmatically picks a random song as current without mutating list order.
     * @return Pointer to selected song or nullptr if empty.
     */
    SongNode* shuffle();

    /**
     * @brief Sets the currently active song by ID.
     */
    bool selectSong(int id);

    /**
     * @brief Advances to the next song in the playlist.
     */
    bool playNext();

    /**
     * @brief Rewinds to the previous song in the playlist.
     */
    bool playPrevious();

    /**
     * @brief Gets the currently active song node.
     */
    SongNode* getCurrentSong() const;

    /**
     * @brief Returns total song count.
     */
    int getSongCount() const;

    /**
     * @brief Returns the data file path being used.
     */
    std::string getDataFilePath() const;
};

#endif // PLAYLISTMANAGER_H
