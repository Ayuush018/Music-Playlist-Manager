#include "MainWindow.h"
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      manager("data/playlist.txt"),
      songTable(nullptr),
      searchInput(nullptr),
      totalSongsLabel(nullptr),
      emptyStateLabel(nullptr),
      currentTitleLabel(nullptr),
      currentArtistLabel(nullptr),
      currentStatusLabel(nullptr),
      prevBtn(nullptr),
      nextBtn(nullptr),
      shuffleBtn(nullptr),
      sidebarCountLabel(nullptr),
      sidebarStorageLabel(nullptr) {
    setWindowTitle("Music Playlist Manager — Advanced Data Structures (C++17 & Qt 6)");
    setMinimumSize(900, 600);
    resize(1200, 750);

    // Main root widget
    QWidget* rootWidget = new QWidget(this);
    setCentralWidget(rootWidget);

    QVBoxLayout* rootLayout = new QVBoxLayout(rootWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Upper body: Sidebar + Central Content
    QWidget* bodyWidget = new QWidget(rootWidget);
    QHBoxLayout* bodyLayout = new QHBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    QWidget* sidebar = createSidebar();
    QWidget* centralContent = createCentralContent();

    bodyLayout->addWidget(sidebar);
    bodyLayout->addWidget(centralContent, 1);

    // Lower body: Bottom Now-Selected Bar
    QWidget* bottomPanel = createBottomPanel();

    rootLayout->addWidget(bodyWidget, 1);
    rootLayout->addWidget(bottomPanel);

    applySpotifyDarkTheme();

    // Initial data synchronization
    refreshTable();
    updateCurrentSongDisplay();
    updateSongCountDisplay();
}

QWidget* MainWindow::createSidebar() {
    QFrame* sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);

    QVBoxLayout* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(16, 24, 16, 24);
    layout->setSpacing(8);

    // Brand / App Title
    QLabel* brandLabel = new QLabel("♫  MUSIC MANAGER");
    brandLabel->setObjectName("brandLabel");
    layout->addWidget(brandLabel);

    layout->addSpacing(18);

    // Navigation Items
    QPushButton* homeBtn = new QPushButton("  🏠  Home");
    homeBtn->setObjectName("navHome");
    homeBtn->setCursor(Qt::PointingHandCursor);

    QPushButton* playlistBtn = new QPushButton("  🎵  My Playlist");
    playlistBtn->setObjectName("navPlaylistActive");
    playlistBtn->setCursor(Qt::PointingHandCursor);

    QPushButton* allSongsBtn = new QPushButton("  📑  All Songs");
    allSongsBtn->setObjectName("navAllSongs");
    allSongsBtn->setCursor(Qt::PointingHandCursor);

    QPushButton* searchNavBtn = new QPushButton("  🔍  Search Library");
    searchNavBtn->setObjectName("navSearch");
    searchNavBtn->setCursor(Qt::PointingHandCursor);
    connect(searchNavBtn, &QPushButton::clicked, this, [this]() {
        if (searchInput) searchInput->setFocus();
    });

    layout->addWidget(homeBtn);
    layout->addWidget(playlistBtn);
    layout->addWidget(allSongsBtn);
    layout->addWidget(searchNavBtn);

    layout->addSpacing(24);

    // Playlist Information Section
    QLabel* sectionHeader = new QLabel("PLAYLIST INFO");
    sectionHeader->setObjectName("sidebarSectionHeader");
    layout->addWidget(sectionHeader);

    sidebarCountLabel = new QLabel("Total Songs: 0");
    sidebarCountLabel->setObjectName("sidebarInfoText");
    layout->addWidget(sidebarCountLabel);

    sidebarStorageLabel = new QLabel("File: data/playlist.txt");
    sidebarStorageLabel->setObjectName("sidebarInfoText");
    layout->addWidget(sidebarStorageLabel);

    QLabel* engineLabel = new QLabel("Engine: Custom DLL (ADSA)");
    engineLabel->setObjectName("sidebarInfoMuted");
    layout->addWidget(engineLabel);

    layout->addStretch();

    // Footer note
    QLabel* noteLabel = new QLabel("Double-click any track\nto select as active.");
    noteLabel->setObjectName("sidebarTipText");
    noteLabel->setWordWrap(true);
    layout->addWidget(noteLabel);

    return sidebar;
}

QWidget* MainWindow::createCentralContent() {
    QWidget* centralWidget = new QWidget();
    centralWidget->setObjectName("centralContent");

    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(28, 24, 28, 16);
    layout->setSpacing(16);

    // Top Header: Title and Subtitle
    QVBoxLayout* headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(4);

    QLabel* titleLabel = new QLabel("My Music Collection");
    titleLabel->setObjectName("headerTitle");

    QLabel* subtitleLabel = new QLabel("Powered by Custom Doubly Linked List with In-Place Pointer Sorting");
    subtitleLabel->setObjectName("headerSubtitle");

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);
    layout->addLayout(headerLayout);

    // Action Toolbar: Search + Add + Shuffle + Sort
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(12);

    searchInput = new QLineEdit();
    searchInput->setObjectName("searchInput");
    searchInput->setPlaceholderText("🔍  Search by song title or artist...");
    searchInput->setClearButtonEnabled(true);
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    toolbarLayout->addWidget(searchInput, 1);

    QPushButton* addBtn = new QPushButton("＋ Add Song");
    addBtn->setObjectName("addSongBtn");
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddSongClicked);
    toolbarLayout->addWidget(addBtn);

    QPushButton* shuffleToolbarBtn = new QPushButton("🔀 Shuffle");
    shuffleToolbarBtn->setObjectName("secondaryActionBtn");
    shuffleToolbarBtn->setCursor(Qt::PointingHandCursor);
    connect(shuffleToolbarBtn, &QPushButton::clicked, this, &MainWindow::onShuffleClicked);
    toolbarLayout->addWidget(shuffleToolbarBtn);

    QPushButton* sortBtn = new QPushButton("🔤 Sort (A-Z)");
    sortBtn->setObjectName("secondaryActionBtn");
    sortBtn->setCursor(Qt::PointingHandCursor);
    connect(sortBtn, &QPushButton::clicked, this, &MainWindow::onSortClicked);
    toolbarLayout->addWidget(sortBtn);

    layout->addLayout(toolbarLayout);

    // Song Table Container with Empty State Overlay
    QFrame* tableContainer = new QFrame();
    tableContainer->setObjectName("tableContainer");
    QVBoxLayout* tableLayout = new QVBoxLayout(tableContainer);
    tableLayout->setContentsMargins(0, 0, 0, 0);

    songTable = new QTableWidget();
    songTable->setObjectName("songTable");
    songTable->setColumnCount(4);
    songTable->setHorizontalHeaderLabels({"ID", "TITLE", "ARTIST", "ACTION"});
    songTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    songTable->setColumnWidth(0, 70);
    songTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    songTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    songTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    songTable->setColumnWidth(3, 110);
    songTable->verticalHeader()->setVisible(false);
    songTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    songTable->setSelectionMode(QAbstractItemView::SingleSelection);
    songTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    songTable->setShowGrid(false);

    connect(songTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableCellDoubleClicked);

    emptyStateLabel = new QLabel("Your playlist is empty.\nClick \"＋ Add Song\" to start your collection!", songTable);
    emptyStateLabel->setObjectName("emptyStateLabel");
    emptyStateLabel->setAlignment(Qt::AlignCenter);
    emptyStateLabel->setVisible(false);

    tableLayout->addWidget(songTable);
    layout->addWidget(tableContainer, 1);

    return centralWidget;
}

QWidget* MainWindow::createBottomPanel() {
    QFrame* bottomPanel = new QFrame();
    bottomPanel->setObjectName("bottomPanel");
    bottomPanel->setFixedHeight(90);

    QHBoxLayout* layout = new QHBoxLayout(bottomPanel);
    layout->setContentsMargins(24, 12, 24, 12);
    layout->setSpacing(20);

    // Left: Now Selected Song Metadata
    QVBoxLayout* songInfoLayout = new QVBoxLayout();
    songInfoLayout->setSpacing(2);
    songInfoLayout->setAlignment(Qt::AlignVCenter);

    QLabel* tagLabel = new QLabel("NOW SELECTED");
    tagLabel->setObjectName("nowSelectedTag");

    currentTitleLabel = new QLabel("No Song Selected");
    currentTitleLabel->setObjectName("currentTitleLabel");

    currentArtistLabel = new QLabel("—");
    currentArtistLabel->setObjectName("currentArtistLabel");

    songInfoLayout->addWidget(tagLabel);
    songInfoLayout->addWidget(currentTitleLabel);
    songInfoLayout->addWidget(currentArtistLabel);
    layout->addLayout(songInfoLayout, 1);

    // Center: Playback Controls (Previous, Next, Shuffle)
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(16);
    controlsLayout->setAlignment(Qt::AlignCenter);

    prevBtn = new QPushButton("⏮  Prev");
    prevBtn->setObjectName("controlBtn");
    prevBtn->setCursor(Qt::PointingHandCursor);
    connect(prevBtn, &QPushButton::clicked, this, &MainWindow::onPreviousClicked);

    shuffleBtn = new QPushButton("🔀  Shuffle");
    shuffleBtn->setObjectName("controlBtn");
    shuffleBtn->setCursor(Qt::PointingHandCursor);
    connect(shuffleBtn, &QPushButton::clicked, this, &MainWindow::onShuffleClicked);

    nextBtn = new QPushButton("Next  ⏭");
    nextBtn->setObjectName("controlBtn");
    nextBtn->setCursor(Qt::PointingHandCursor);
    connect(nextBtn, &QPushButton::clicked, this, &MainWindow::onNextClicked);

    controlsLayout->addWidget(prevBtn);
    controlsLayout->addWidget(shuffleBtn);
    controlsLayout->addWidget(nextBtn);
    layout->addLayout(controlsLayout, 1);

    // Right: Status / Doubly Linked List details
    QVBoxLayout* statusLayout = new QVBoxLayout();
    statusLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusLayout->setSpacing(2);

    currentStatusLabel = new QLabel("Backend: Synced");
    currentStatusLabel->setObjectName("currentStatusLabel");
    statusLayout->addWidget(currentStatusLabel);

    totalSongsLabel = new QLabel("Tracks: 0");
    totalSongsLabel->setObjectName("totalSongsLabel");
    statusLayout->addWidget(totalSongsLabel);

    layout->addLayout(statusLayout, 1);

    return bottomPanel;
}

void MainWindow::applySpotifyDarkTheme() {
    QString qss = R"(
        /* Global Window Styling */
        QMainWindow {
            background-color: #121212;
            color: #FFFFFF;
            font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, Roboto, Helvetica, Arial, sans-serif;
        }

        QWidget {
            background-color: transparent;
            color: #FFFFFF;
        }

        /* Left Sidebar */
        #sidebar {
            background-color: #000000;
            border-right: 1px solid #1f1f1f;
        }

        #brandLabel {
            color: #1DB954;
            font-size: 16px;
            font-weight: 800;
            letter-spacing: 1px;
        }

        #navHome, #navPlaylistActive, #navAllSongs, #navSearch {
            text-align: left;
            padding: 10px 14px;
            border-radius: 6px;
            font-size: 13px;
            font-weight: 600;
            border: none;
            color: #B3B3B3;
        }

        #navHome:hover, #navAllSongs:hover, #navSearch:hover {
            color: #FFFFFF;
            background-color: #1A1A1A;
        }

        #navPlaylistActive {
            color: #1DB954;
            background-color: #1A1A1A;
            border-left: 3px solid #1DB954;
        }

        #sidebarSectionHeader {
            color: #666666;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1.5px;
            margin-top: 10px;
        }

        #sidebarInfoText {
            color: #CCCCCC;
            font-size: 12px;
            padding: 2px 0;
        }

        #sidebarInfoMuted {
            color: #777777;
            font-size: 11px;
        }

        #sidebarTipText {
            color: #555555;
            font-size: 11px;
            line-height: 1.4;
        }

        /* Central Content Area */
        #centralContent {
            background-color: #121212;
        }

        #headerTitle {
            font-size: 26px;
            font-weight: 800;
            color: #FFFFFF;
        }

        #headerSubtitle {
            font-size: 13px;
            color: #A0A0A0;
        }

        /* Toolbar Components */
        #searchInput {
            background-color: #242424;
            border: 1px solid #333333;
            border-radius: 20px;
            padding: 8px 16px;
            font-size: 13px;
            color: #FFFFFF;
            selection-background-color: #1DB954;
        }

        #searchInput:focus {
            border: 1px solid #1DB954;
            background-color: #2A2A2A;
        }

        #addSongBtn {
            background-color: #1DB954;
            color: #000000;
            font-size: 13px;
            font-weight: 700;
            border-radius: 20px;
            padding: 8px 20px;
            border: none;
        }

        #addSongBtn:hover {
            background-color: #1ED760;
        }

        #addSongBtn:pressed {
            background-color: #169C46;
        }

        #secondaryActionBtn {
            background-color: #242424;
            color: #FFFFFF;
            font-size: 13px;
            font-weight: 600;
            border: 1px solid #383838;
            border-radius: 20px;
            padding: 8px 16px;
        }

        #secondaryActionBtn:hover {
            background-color: #333333;
            border-color: #555555;
        }

        /* Table Styling */
        #tableContainer {
            background-color: #181818;
            border-radius: 8px;
            border: 1px solid #242424;
        }

        QTableWidget {
            background-color: #181818;
            border: none;
            outline: none;
        }

        QHeaderView::section {
            background-color: #181818;
            color: #B3B3B3;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            padding: 8px 12px;
            border: none;
            border-bottom: 1px solid #282828;
        }

        QTableWidget::item {
            padding: 8px 12px;
            border-bottom: 1px solid #202020;
            color: #FFFFFF;
            font-size: 13px;
        }

        QTableWidget::item:selected {
            background-color: #282828;
            color: #1DB954;
        }

        QTableWidget::item:hover {
            background-color: #202020;
        }

        .deleteBtn {
            background-color: #2A1F1F;
            color: #E05D5D;
            border: 1px solid #5A2A2A;
            border-radius: 4px;
            padding: 4px 8px;
            font-size: 11px;
            font-weight: 600;
        }

        .deleteBtn:hover {
            background-color: #D32F2F;
            color: #FFFFFF;
            border-color: #D32F2F;
        }

        #emptyStateLabel {
            color: #666666;
            font-size: 15px;
            font-weight: 500;
            line-height: 1.6;
        }

        /* Bottom Selected Song Bar */
        #bottomPanel {
            background-color: #181818;
            border-top: 1px solid #242424;
        }

        #nowSelectedTag {
            color: #1DB954;
            font-size: 10px;
            font-weight: 800;
            letter-spacing: 1.5px;
        }

        #currentTitleLabel {
            font-size: 15px;
            font-weight: 700;
            color: #FFFFFF;
        }

        #currentArtistLabel {
            font-size: 12px;
            color: #B3B3B3;
        }

        #controlBtn {
            background-color: #282828;
            color: #FFFFFF;
            border-radius: 18px;
            padding: 8px 16px;
            font-size: 12px;
            font-weight: 600;
            border: 1px solid #383838;
        }

        #controlBtn:hover {
            background-color: #383838;
            border-color: #1DB954;
        }

        #currentStatusLabel {
            color: #1DB954;
            font-size: 11px;
            font-weight: 600;
        }

        #totalSongsLabel {
            color: #888888;
            font-size: 12px;
        }

        /* Scrollbars */
        QScrollBar:vertical {
            background: #121212;
            width: 8px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background: #333333;
            min-height: 20px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical:hover {
            background: #555555;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    )";

    setStyleSheet(qss);
}

void MainWindow::refreshTable(const QString& filterText) {
    if (!songTable) return;

    songTable->setRowCount(0);
    SongNode* curr = manager.getPlaylist().getHead();
    SongNode* activeSong = manager.getCurrentSong();
    QString filter = filterText.trimmed();

    int row = 0;
    while (curr != nullptr) {
        QString title = QString::fromStdString(curr->title);
        QString artist = QString::fromStdString(curr->artist);

        bool matches = filter.isEmpty() ||
                       title.contains(filter, Qt::CaseInsensitive) ||
                       artist.contains(filter, Qt::CaseInsensitive);

        if (matches) {
            songTable->insertRow(row);

            // Column 0: ID
            QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(curr->id));
            idItem->setTextAlignment(Qt::AlignCenter);
            idItem->setData(Qt::UserRole, curr->id);

            // Column 1: Title (prefixed with icon if active)
            QString displayTitle = (curr == activeSong) ? "▶  " + title : title;
            QTableWidgetItem* titleItem = new QTableWidgetItem(displayTitle);

            // Column 2: Artist
            QTableWidgetItem* artistItem = new QTableWidgetItem(artist);

            // Column 3: Delete Action Button
            QWidget* actionWidget = new QWidget();
            QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
            actionLayout->setContentsMargins(4, 2, 4, 2);
            actionLayout->setAlignment(Qt::AlignCenter);

            QPushButton* delBtn = new QPushButton("🗑 Delete");
            delBtn->setProperty("class", "deleteBtn");
            delBtn->setCursor(Qt::PointingHandCursor);
            int songId = curr->id;
            connect(delBtn, &QPushButton::clicked, this, [this, songId]() {
                onDeleteSongClicked(songId);
            });
            actionLayout->addWidget(delBtn);

            songTable->setItem(row, 0, idItem);
            songTable->setItem(row, 1, titleItem);
            songTable->setItem(row, 2, artistItem);
            songTable->setCellWidget(row, 3, actionWidget);

            // Highlight row if active song
            if (curr == activeSong) {
                idItem->setForeground(QBrush(QColor("#1DB954")));
                titleItem->setForeground(QBrush(QColor("#1DB954")));
                QFont f = titleItem->font();
                f.setBold(true);
                titleItem->setFont(f);
                artistItem->setForeground(QBrush(QColor("#1DB954")));
                songTable->selectRow(row);
            }

            row++;
        }
        curr = curr->next;
    }

    if (emptyStateLabel) {
        emptyStateLabel->setGeometry(songTable->rect());
        emptyStateLabel->setVisible(row == 0);
    }
}

void MainWindow::updateCurrentSongDisplay() {
    SongNode* active = manager.getCurrentSong();
    if (active) {
        currentTitleLabel->setText(QString::fromStdString(active->title));
        currentArtistLabel->setText(QString::fromStdString(active->artist));
        currentStatusLabel->setText(QString("Selected ID: %1 • Node in Memory").arg(active->id));
    } else {
        currentTitleLabel->setText("No Song Selected");
        currentArtistLabel->setText("—");
        currentStatusLabel->setText("Playlist Empty");
    }
}

void MainWindow::updateSongCountDisplay() {
    int count = manager.getSongCount();
    if (totalSongsLabel) totalSongsLabel->setText(QString("Total Tracks: %1").arg(count));
    if (sidebarCountLabel) sidebarCountLabel->setText(QString("Total Songs: %1").arg(count));
    if (sidebarStorageLabel) sidebarStorageLabel->setText(QString("File: %1").arg(QString::fromStdString(manager.getDataFilePath())));
}

void MainWindow::onAddSongClicked() {
    AddSongDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString title = dialog.getTitle().trimmed();
        QString artist = dialog.getArtist().trimmed();

        if (title.isEmpty() || artist.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Song title and artist name cannot be empty.");
            return;
        }

        int newId = manager.addSong(title.toStdString(), artist.toStdString());
        if (newId != -1) {
            refreshTable(searchInput ? searchInput->text() : "");
            updateCurrentSongDisplay();
            updateSongCountDisplay();
            statusBar()->showMessage(QString("Added \"%1\" with ID %2").arg(title).arg(newId), 3000);
        }
    }
}

void MainWindow::onDeleteSongClicked(int songId) {
    SongNode* target = manager.getPlaylist().findById(songId);
    if (!target) return;

    QString songName = QString::fromStdString(target->title);
    QString artistName = QString::fromStdString(target->artist);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirm Deletion",
        QString("Are you sure you want to remove \"%1\" by %2 (ID: %3) from the playlist?\n\nThis will update linked-list pointers and persist to disk.")
            .arg(songName).arg(artistName).arg(songId),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        bool deleted = manager.deleteSong(songId);
        if (deleted) {
            refreshTable(searchInput ? searchInput->text() : "");
            updateCurrentSongDisplay();
            updateSongCountDisplay();
            statusBar()->showMessage(QString("Deleted ID %1 (\"%2\")").arg(songId).arg(songName), 3000);
        }
    }
}

void MainWindow::onSearchTextChanged(const QString& text) {
    refreshTable(text);
}

void MainWindow::onShuffleClicked() {
    if (manager.getPlaylist().isEmpty()) {
        QMessageBox::information(this, "Playlist Empty", "Cannot shuffle an empty playlist.");
        return;
    }

    SongNode* picked = manager.shuffle();
    if (picked) {
        refreshTable(searchInput ? searchInput->text() : "");
        updateCurrentSongDisplay();
        statusBar()->showMessage(QString("Shuffled to: \"%1\" by %2").arg(QString::fromStdString(picked->title)).arg(QString::fromStdString(picked->artist)), 3000);
    }
}

void MainWindow::onSortClicked() {
    if (manager.getPlaylist().isEmpty()) {
        QMessageBox::information(this, "Playlist Empty", "Cannot sort an empty playlist.");
        return;
    }

    manager.sortPlaylist();
    refreshTable(searchInput ? searchInput->text() : "");
    updateCurrentSongDisplay();
    statusBar()->showMessage("Playlist sorted alphabetically (A-Z) via linked-list pointer swaps.", 3000);
}

void MainWindow::onNextClicked() {
    if (manager.getPlaylist().isEmpty()) return;

    if (manager.playNext()) {
        refreshTable(searchInput ? searchInput->text() : "");
        updateCurrentSongDisplay();
    } else {
        statusBar()->showMessage("Boundary reached: You are at the last track in the playlist.", 2500);
    }
}

void MainWindow::onPreviousClicked() {
    if (manager.getPlaylist().isEmpty()) return;

    if (manager.playPrevious()) {
        refreshTable(searchInput ? searchInput->text() : "");
        updateCurrentSongDisplay();
    } else {
        statusBar()->showMessage("Boundary reached: You are at the first track in the playlist.", 2500);
    }
}

void MainWindow::onTableCellDoubleClicked(int row, int /*column*/) {
    if (!songTable) return;
    QTableWidgetItem* idItem = songTable->item(row, 0);
    if (!idItem) return;

    int songId = idItem->data(Qt::UserRole).toInt();
    if (manager.selectSong(songId)) {
        refreshTable(searchInput ? searchInput->text() : "");
        updateCurrentSongDisplay();
        statusBar()->showMessage(QString("Selected track ID %1 as active.").arg(songId), 2500);
    }
}

void MainWindow::onTableSelectionChanged() {
    // Selection highlight helper
}

// ==========================================
// AddSongDialog Implementation
// ==========================================
AddSongDialog::AddSongDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Add Song to Playlist");
    setFixedSize(380, 240);
    setStyleSheet(R"(
        QDialog {
            background-color: #1A1A1A;
            color: #FFFFFF;
        }
        QLabel {
            color: #CCCCCC;
            font-size: 13px;
            font-weight: 600;
        }
        QLineEdit {
            background-color: #282828;
            border: 1px solid #3E3E3E;
            border-radius: 6px;
            padding: 8px 12px;
            color: #FFFFFF;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 1px solid #1DB954;
        }
        QPushButton#confirmBtn {
            background-color: #1DB954;
            color: #000000;
            font-weight: 700;
            border-radius: 18px;
            padding: 8px 18px;
            border: none;
        }
        QPushButton#confirmBtn:hover {
            background-color: #1ED760;
        }
        QPushButton#cancelBtn {
            background-color: #2C2C2C;
            color: #FFFFFF;
            border-radius: 18px;
            padding: 8px 18px;
            border: 1px solid #444444;
        }
        QPushButton#cancelBtn:hover {
            background-color: #383838;
        }
    )");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);

    layout->addWidget(new QLabel("Song Title:"));
    titleEdit = new QLineEdit();
    titleEdit->setPlaceholderText("e.g. Bohemian Rhapsody");
    layout->addWidget(titleEdit);

    layout->addWidget(new QLabel("Artist Name:"));
    artistEdit = new QLineEdit();
    artistEdit->setPlaceholderText("e.g. Queen");
    layout->addWidget(artistEdit);

    layout->addSpacing(10);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    QPushButton* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("cancelBtn");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton* confirmBtn = new QPushButton("Add Song");
    confirmBtn->setObjectName("confirmBtn");
    connect(confirmBtn, &QPushButton::clicked, this, [this]() {
        if (titleEdit->text().trimmed().isEmpty() || artistEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "Please fill in both the Song Title and Artist Name.");
            return;
        }
        accept();
    });

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);

    layout->addLayout(btnLayout);
}

QString AddSongDialog::getTitle() const {
    return titleEdit->text();
}

QString AddSongDialog::getArtist() const {
    return artistEdit->text();
}
