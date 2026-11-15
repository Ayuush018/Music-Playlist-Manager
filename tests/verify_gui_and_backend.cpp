#include <QApplication>
#include <QTableWidget>
#include <QLineEdit>
#include <cassert>
#include <iostream>
#include <fstream>
#include "MainWindow.h"
#include "PlaylistManager.h"
#include "DoublyLinkedList.h"

int main(int argc, char* argv[]) {
    // Run in offscreen mode for automated CI/headless execution
    qputenv("QT_QPA_PLATFORM", QByteArray("offscreen"));
    QApplication app(argc, argv);

    std::cout << "========================================================\n";
    std::cout << "  RUNNING GUI & BACKEND INTEGRATION TEST SUITE (QT 6)   \n";
    std::cout << "========================================================\n\n";

    // 1. Verify custom DoublyLinkedList is the core data structure
    {
        DoublyLinkedList list;
        list.addSong(101, "Test Track 1", "Artist One");
        list.addSong(102, "Test Track 2", "Artist Two");
        list.addSong(103, "Test Track 3", "Artist Three");
        assert(list.getSize() == 3);
        assert(list.validateIntegrity());
        assert(list.getHead()->id == 101);
        assert(list.getHead()->next->id == 102);
        assert(list.getHead()->next->next->id == 103);
        assert(list.getHead()->next->next->prev->id == 102);
        std::cout << "[PASS] 1. Custom Doubly Linked List raw pointers verified.\n";
    }

    // 2. Test File Persistence with PlaylistManager
    std::string testPath = "data/test_gui_sync.txt";
    std::remove(testPath.c_str());
    {
        PlaylistManager pm(testPath);
        int id1 = pm.addSong("Synchronized Song A", "Artist A");
        int id2 = pm.addSong("Synchronized Song B", "Artist B");
        assert(id1 == 1);
        assert(id2 == 2);
        assert(pm.getSongCount() == 2);

        // Verify file was written
        std::ifstream file(testPath);
        assert(file.is_open());
        file.close();
        std::cout << "[PASS] 2. PlaylistManager file creation and persistence verified.\n";
    }

    // 3. Test In-Place Sorting on Doubly Linked List
    {
        PlaylistManager pm(testPath);
        pm.addSong("Zeta Track", "Artist Z");
        pm.addSong("Alpha Track", "Artist A");
        pm.addSong("Beta Track", "Artist B");
        assert(pm.getPlaylist().validateIntegrity());

        // Sort in place
        pm.sortPlaylist();
        assert(pm.getPlaylist().validateIntegrity());

        // Check head is Alpha
        SongNode* head = pm.getPlaylist().getHead();
        assert(head != nullptr);
        assert(head->title == "Alpha Track");
        std::cout << "[PASS] 3. In-place linked-list sorting verified (Title: Alpha Track at head).\n";
    }

    // 4. Test Navigation and Shuffle
    {
        PlaylistManager pm(testPath);
        SongNode* initial = pm.getCurrentSong();
        assert(initial != nullptr);

        bool movedNext = pm.playNext();
        assert(movedNext);
        assert(pm.getCurrentSong() != initial);

        bool movedPrev = pm.playPrevious();
        assert(movedPrev);
        assert(pm.getCurrentSong() == initial);

        // Test boundary
        assert(!pm.playPrevious()); // Cannot move before head

        // Test shuffle
        SongNode* shuffled = pm.shuffle();
        assert(shuffled != nullptr);
        assert(pm.getCurrentSong() == shuffled);
        assert(pm.getPlaylist().validateIntegrity());
        std::cout << "[PASS] 4. Navigation (prev/next) and Shuffle verified.\n";
    }

    // 5. Test Deletion and Selection Update
    {
        PlaylistManager pm(testPath);
        int initialCount = pm.getSongCount();
        bool deleted = pm.deleteSong(1); // delete first added
        assert(deleted);
        assert(pm.getSongCount() == initialCount - 1);
        assert(pm.getPlaylist().findById(1) == nullptr);
        assert(pm.getPlaylist().validateIntegrity());
        std::cout << "[PASS] 5. Song deletion and pointer reconnection verified.\n";
    }

    // 6. Test MainWindow instantiation and Table Synchronization
    {
        MainWindow win;
        win.show();

        // Find child widgets via QObject
        QTableWidget* table = win.findChild<QTableWidget*>("songTable");
        QLineEdit* search = win.findChild<QLineEdit*>("searchInput");
        assert(table != nullptr);
        assert(search != nullptr);

        int rowCount = table->rowCount();
        std::cout << "[INFO] MainWindow loaded with " << rowCount << " rows from active playlist.\n";
        assert(rowCount >= 0);

        // Test search filter
        search->setText("NonExistentXYZ999");
        QCoreApplication::processEvents();
        assert(table->rowCount() == 0);

        // Clear search
        search->clear();
        QCoreApplication::processEvents();
        assert(table->rowCount() == rowCount);
        std::cout << "[PASS] 6. MainWindow GUI table and search filter verified." << std::endl;
    }

    // Clean up temporary test file
    std::remove(testPath.c_str());

    std::cout << "\n========================================================\n"
              << "  ALL GUI & BACKEND INTEGRATION TESTS PASSED (100%)!   \n"
              << "========================================================" << std::endl;

    return 0;
}
