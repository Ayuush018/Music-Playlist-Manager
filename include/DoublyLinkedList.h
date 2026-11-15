#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include "Song.h"
#include <string>
#include <random>

/**
 * @brief Custom Doubly Linked List implementation managing SongNode elements.
 * 
 * Complies with strict pointer manipulation requirements: no STL containers
 * are used for internal storage. All allocations and deallocations are
 * handled manually via new/delete.
 */
class DoublyLinkedList {
private:
    SongNode* head;      ///< Pointer to first node in the list
    SongNode* tail;      ///< Pointer to last node in the list
    SongNode* current;   ///< Pointer to currently active / playing song
    int size;            ///< Total count of nodes currently in list

    // Random number generator engine and distribution
    mutable std::mt19937 rng;

    /**
     * @brief Swaps two adjacent nodes in the doubly linked list by pointer manipulation.
     * @param first The predecessor node (first->next must equal second).
     * @param second The successor node (second->prev must equal first).
     */
    void swapAdjacentNodes(SongNode* first, SongNode* second);

    /**
     * @brief Compares two strings case-insensitively.
     * @param a First string.
     * @param b Second string.
     * @return Negative if a < b, 0 if a == b, Positive if a > b.
     */
    static int compareCaseInsensitive(const std::string& a, const std::string& b);

    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param str Input string.
     * @return Trimmed string.
     */
    static std::string trimString(const std::string& str);

public:
    /**
     * @brief Default constructor. Initializes empty doubly linked list.
     */
    DoublyLinkedList();

    /**
     * @brief Destructor. Releases all dynamically allocated nodes.
     */
    ~DoublyLinkedList();

    // Prevent accidental shallow copying (Rule of Three/Five)
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    // Enable move semantics
    DoublyLinkedList(DoublyLinkedList&& other) noexcept;
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept;

    /**
     * @brief Appends a new song to the end of the playlist.
     * @param id Unique song identifier.
     * @param title Title of song.
     * @param artist Artist of song.
     */
    void addSong(int id, const std::string& title, const std::string& artist);

    /**
     * @brief Deletes a song from the playlist by its ID.
     * Updates head, tail, and current pointers as needed.
     * @param id Unique song identifier to remove.
     * @return true if deleted, false if not found.
     */
    bool removeSong(int id);

    /**
     * @brief Searches for a song node by exact ID.
     * @param id Song ID.
     * @return Pointer to node if found, nullptr otherwise.
     */
    SongNode* findById(int id) const;

    /**
     * @brief Searches for a song node by title (case-insensitive & trimmed).
     * @param title Song title to search.
     * @return Pointer to node if found, nullptr otherwise.
     */
    SongNode* findByTitle(const std::string& title) const;

    /**
     * @brief Displays playlist from head to tail.
     */
    void displayForward() const;

    /**
     * @brief Displays playlist from tail to head.
     */
    void displayBackward() const;

    /**
     * @brief Advances current selection to the successor node.
     * @return true if moved successfully, false if already at tail or list is empty.
     */
    bool nextSong();

    /**
     * @brief Moves current selection to the predecessor node.
     * @return true if moved successfully, false if already at head or list is empty.
     */
    bool previousSong();

    /**
     * @brief Returns pointer to the currently selected song.
     * @return SongNode* or nullptr if playlist is empty.
     */
    SongNode* getCurrentSong() const;

    /**
     * @brief Returns total number of songs in the playlist.
     */
    int getSize() const;

    /**
     * @brief Checks whether the playlist is empty.
     */
    bool isEmpty() const;

    /**
     * @brief Sorts the playlist alphabetically by song title in ascending order.
     * Performs sorting in-place by rearranging linked-list pointers.
     * Preserves the validity of current song pointer.
     */
    void sortByTitle();

    /**
     * @brief Randomly selects a song as current without modifying node order.
     * @return Pointer to the newly selected current song, or nullptr if empty.
     */
    SongNode* getRandomSong();

    /**
     * @brief Deallocates all nodes and resets playlist to empty state.
     */
    void clear();

    /**
     * @brief Sets the current song pointer directly (e.g. on loading).
     * @param node Target node.
     */
    void setCurrent(SongNode* node);

    /**
     * @brief Sets the current song pointer by unique Song ID.
     * @param id Song ID to set as current.
     * @return true if found and set, false otherwise.
     */
    bool setCurrentById(int id);

    /**
     * @brief Gets the head node pointer (read-only for serialization/traversal).
     */
    SongNode* getHead() const;

    /**
     * @brief Internal integrity validation check for development and test assertions.
     * Checks head/tail boundaries, bidirectional pointer consistency, size match,
     * and current pointer reachability.
     * @return true if linked list integrity is intact, false otherwise.
     */
    bool validateIntegrity() const;
};

#endif // DOUBLYLINKEDLIST_H
