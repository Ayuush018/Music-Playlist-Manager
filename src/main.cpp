#include "PlaylistManager.h"
#include <iostream>

int main() {
    try {
        PlaylistManager manager("data/playlist.txt");
        manager.run();
    } catch (const std::exception& e) {
        std::cerr << "\n[FATAL ERROR] An unexpected exception occurred: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[FATAL ERROR] An unknown critical error occurred.\n";
        return 1;
    }
    return 0;
}
