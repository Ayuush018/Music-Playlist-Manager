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
};

#endif // PLAYLISTMANAGER_H
