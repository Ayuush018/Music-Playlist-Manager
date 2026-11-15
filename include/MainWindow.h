#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QFrame>
#include "PlaylistManager.h"

/**
 * @brief Spotify-inspired Modern Desktop GUI for Music Playlist Manager.
 * 
 * Powered directly by the custom DoublyLinkedList backend via PlaylistManager.
 * Does not replicate or duplicate playlist storage.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    PlaylistManager manager; ///< Backend playlist coordinator

    // UI Widgets - Central Table & Toolbar
    QTableWidget* songTable;
    QLineEdit* searchInput;
    QLabel* totalSongsLabel;
    QLabel* emptyStateLabel;

    // UI Widgets - Bottom Now-Selected Panel
    QLabel* currentTitleLabel;
    QLabel* currentArtistLabel;
    QLabel* currentStatusLabel;
    QPushButton* prevBtn;
    QPushButton* nextBtn;
    QPushButton* shuffleBtn;

    // UI Widgets - Sidebar
    QLabel* sidebarCountLabel;
    QLabel* sidebarStorageLabel;

    // Layout setup helpers
    QWidget* createSidebar();
    QWidget* createCentralContent();
    QWidget* createBottomPanel();
    void applySpotifyDarkTheme();

    // Data synchronization helpers
    void refreshTable(const QString& filterText = QString());
    void updateCurrentSongDisplay();
    void updateSongCountDisplay();

private slots:
    void onAddSongClicked();
    void onDeleteSongClicked(int songId);
    void onSearchTextChanged(const QString& text);
    void onShuffleClicked();
    void onSortClicked();
    void onNextClicked();
    void onPreviousClicked();
    void onTableCellDoubleClicked(int row, int column);
    void onTableSelectionChanged();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
};

/**
 * @brief Modal dialog to add a new song to the playlist.
 */
class AddSongDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* titleEdit;
    QLineEdit* artistEdit;

public:
    explicit AddSongDialog(QWidget* parent = nullptr);
    QString getTitle() const;
    QString getArtist() const;
};

#endif // MAINWINDOW_H
