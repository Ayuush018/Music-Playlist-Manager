#ifndef SONG_H
#define SONG_H

#include <string>
#include <iostream>

/**
 * @brief Represents a single song node in the doubly linked list playlist.
 * 
 * Each node holds song metadata (id, title, artist) and bidirectional
 * pointers to the previous and next nodes.
 */
struct SongNode {
    int id;
    std::string title;
    std::string artist;
    SongNode* prev;
    SongNode* next;

    /**
     * @brief Parameterized constructor for SongNode.
     * @param songId Unique numeric identifier for the song.
     * @param songTitle Title of the song.
     * @param songArtist Artist / performer name.
     * @param prevNode Pointer to predecessor node (defaults to nullptr).
     * @param nextNode Pointer to successor node (defaults to nullptr).
     */
    SongNode(int songId, std::string songTitle, std::string songArtist,
             SongNode* prevNode = nullptr, SongNode* nextNode = nullptr);

    /**
     * @brief Prints song details formatted in a single line or card.
     * @param isCurrent Flag indicating if this song is currently selected.
     */
    void display(bool isCurrent = false) const;
};

#endif // SONG_H
