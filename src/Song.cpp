#include "Song.h"
#include <iomanip>
#include <iostream>

SongNode::SongNode(int songId, std::string songTitle, std::string songArtist,
                   SongNode* prevNode, SongNode* nextNode)
    : id(songId),
      title(std::move(songTitle)),
      artist(std::move(songArtist)),
      prev(prevNode),
      next(nextNode) {}

void SongNode::display(bool isCurrent) const {
    std::string currentMarker = isCurrent ? "[CURRENT] ▶ " : "            ";
    std::cout << currentMarker
              << "ID: " << std::setw(4) << std::left << id << " | "
              << "Title: " << std::setw(28) << std::left 
              << (title.length() > 27 ? title.substr(0, 24) + "..." : title) << " | "
              << "Artist: " << std::setw(22) << std::left
              << (artist.length() > 21 ? artist.substr(0, 18) + "..." : artist)
              << "\n";
}
