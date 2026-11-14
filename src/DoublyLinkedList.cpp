#include "DoublyLinkedList.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

DoublyLinkedList::DoublyLinkedList()
    : head(nullptr), tail(nullptr), current(nullptr), size(0) {
    std::random_device rd;
    rng = std::mt19937(rd());
}

DoublyLinkedList::~DoublyLinkedList() {
    clear();
}

DoublyLinkedList::DoublyLinkedList(DoublyLinkedList&& other) noexcept
    : head(other.head), tail(other.tail), current(other.current), size(other.size),
      rng(std::move(other.rng)) {
    other.head = nullptr;
    other.tail = nullptr;
    other.current = nullptr;
    other.size = 0;
}

DoublyLinkedList& DoublyLinkedList::operator=(DoublyLinkedList&& other) noexcept {
    if (this != &other) {
        clear();
        head = other.head;
        tail = other.tail;
        current = other.current;
        size = other.size;
        rng = std::move(other.rng);

        other.head = nullptr;
        other.tail = nullptr;
        other.current = nullptr;
        other.size = 0;
    }
    return *this;
}

std::string DoublyLinkedList::trimString(const std::string& str) {
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

int DoublyLinkedList::compareCaseInsensitive(const std::string& a, const std::string& b) {
    std::string s1 = trimString(a);
    std::string s2 = trimString(b);
    size_t len = std::min(s1.size(), s2.size());
    for (size_t i = 0; i < len; ++i) {
        unsigned char c1 = std::tolower(static_cast<unsigned char>(s1[i]));
        unsigned char c2 = std::tolower(static_cast<unsigned char>(s2[i]));
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
    }
    if (s1.size() < s2.size()) return -1;
    if (s1.size() > s2.size()) return 1;
    return 0;
}

void DoublyLinkedList::swapAdjacentNodes(SongNode* first, SongNode* second) {
    if (!first || !second || first->next != second) return;

    SongNode* p = first->prev;
    SongNode* n = second->next;

    // Connect previous node to second
    if (p) {
        p->next = second;
    } else {
        head = second;
    }
    second->prev = p;

    // Connect second to first
    second->next = first;
    first->prev = second;

    // Connect first to next node
    first->next = n;
    if (n) {
        n->prev = first;
    } else {
        tail = first;
    }
}

void DoublyLinkedList::addSong(int id, const std::string& title, const std::string& artist) {
    SongNode* newNode = new SongNode(id, trimString(title), trimString(artist));

    if (isEmpty()) {
        head = newNode;
        tail = newNode;
        current = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
        if (!current) {
            current = head;
        }
    }
    size++;
}

bool DoublyLinkedList::removeSong(int id) {
    SongNode* target = findById(id);
    if (!target) {
        return false;
    }

    // Adjust neighbor pointers
    if (target == head && target == tail) {
        head = nullptr;
        tail = nullptr;
    } else if (target == head) {
        head = head->next;
        head->prev = nullptr;
    } else if (target == tail) {
        tail = tail->prev;
        tail->next = nullptr;
    } else {
        target->prev->next = target->next;
        target->next->prev = target->prev;
    }

    // Selection adjustment: successor if available; otherwise predecessor; else nullptr
    if (current == target) {
        if (target->next) {
            current = target->next;
        } else if (target->prev) {
            current = target->prev;
        } else {
            current = nullptr;
        }
    }

    delete target;
    size--;
    return true;
}

SongNode* DoublyLinkedList::findById(int id) const {
    SongNode* temp = head;
    while (temp != nullptr) {
        if (temp->id == id) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

SongNode* DoublyLinkedList::findByTitle(const std::string& title) const {
    std::string searchKey = trimString(title);
    SongNode* temp = head;
    while (temp != nullptr) {
        if (compareCaseInsensitive(temp->title, searchKey) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

void DoublyLinkedList::displayForward() const {
    if (isEmpty()) {
        std::cout << "\n  [!] The playlist is currently empty.\n";
        return;
    }

    std::cout << "\n========================================================================================\n";
    std::cout << "                               PLAYLIST (FORWARD: HEAD -> TAIL)                         \n";
    std::cout << "========================================================================================\n";
    std::cout << "  Status     ID    Title                        Artist\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    SongNode* temp = head;
    while (temp != nullptr) {
        temp->display(temp == current);
        temp = temp->next;
    }
    std::cout << "========================================================================================\n";
    std::cout << "  Total Songs: " << size << "\n";
}

void DoublyLinkedList::displayBackward() const {
    if (isEmpty()) {
        std::cout << "\n  [!] The playlist is currently empty.\n";
        return;
    }

    std::cout << "\n========================================================================================\n";
    std::cout << "                               PLAYLIST (BACKWARD: TAIL -> HEAD)                        \n";
    std::cout << "========================================================================================\n";
    std::cout << "  Status     ID    Title                        Artist\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    SongNode* temp = tail;
    while (temp != nullptr) {
        temp->display(temp == current);
        temp = temp->prev;
    }
    std::cout << "========================================================================================\n";
    std::cout << "  Total Songs: " << size << "\n";
}

bool DoublyLinkedList::nextSong() {
    if (isEmpty() || !current) {
        return false;
    }
    if (current->next == nullptr) {
        // Boundary reached: already at last song
        return false;
    }
    current = current->next;
    return true;
}

bool DoublyLinkedList::previousSong() {
    if (isEmpty() || !current) {
        return false;
    }
    if (current->prev == nullptr) {
        // Boundary reached: already at first song
        return false;
    }
    current = current->prev;
    return true;
}

SongNode* DoublyLinkedList::getCurrentSong() const {
    return current;
}

int DoublyLinkedList::getSize() const {
    return size;
}

bool DoublyLinkedList::isEmpty() const {
    return head == nullptr;
}

void DoublyLinkedList::sortByTitle() {
    if (size <= 1) {
        return;
    }

    bool swapped;
    do {
        swapped = false;
        SongNode* curr = head;
        while (curr && curr->next) {
            int cmp = compareCaseInsensitive(curr->title, curr->next->title);
            // Tie-break with ID for deterministic order
            if (cmp > 0 || (cmp == 0 && curr->id > curr->next->id)) {
                SongNode* nextNode = curr->next;
                swapAdjacentNodes(curr, nextNode);
                swapped = true;
                // Note: after swapAdjacentNodes, nextNode is predecessor and curr is successor.
                // curr remains in position to be compared with its new next!
            } else {
                curr = curr->next;
            }
        }
    } while (swapped);
}

SongNode* DoublyLinkedList::getRandomSong() {
    if (isEmpty()) {
        return nullptr;
    }
    if (size == 1) {
        current = head;
        return current;
    }

    std::uniform_int_distribution<int> dist(0, size - 1);
    int randomIndex = dist(rng);

    SongNode* temp = head;
    for (int i = 0; i < randomIndex && temp != nullptr; ++i) {
        temp = temp->next;
    }

    if (temp) {
        current = temp;
    }
    return current;
}

void DoublyLinkedList::clear() {
    SongNode* curr = head;
    while (curr != nullptr) {
        SongNode* nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
    head = nullptr;
    tail = nullptr;
    current = nullptr;
    size = 0;
}

void DoublyLinkedList::setCurrent(SongNode* node) {
    current = node;
}

SongNode* DoublyLinkedList::getHead() const {
    return head;
}

bool DoublyLinkedList::validateIntegrity() const {
    if (head == nullptr) {
        return tail == nullptr && current == nullptr && size == 0;
    }

    if (tail == nullptr || size <= 0) {
        return false;
    }

    if (head->prev != nullptr || tail->next != nullptr) {
        return false;
    }

    int count = 0;
    bool currentFound = (current == nullptr);
    SongNode* curr = head;
    SongNode* lastNode = nullptr;

    while (curr != nullptr) {
        count++;
        if (curr == current) {
            currentFound = true;
        }
        if (curr->prev != lastNode) {
            return false;
        }
        lastNode = curr;
        curr = curr->next;
    }

    if (lastNode != tail) {
        return false;
    }

    if (count != size) {
        return false;
    }

    if (!currentFound) {
        return false;
    }

    return true;
}
