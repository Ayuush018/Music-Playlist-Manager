#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Music Playlist Manager");
    app.setApplicationDisplayName("Music Playlist Manager");
    app.setApplicationVersion("1.1.0");

    MainWindow window;
    window.show();

    return app.exec();
}
