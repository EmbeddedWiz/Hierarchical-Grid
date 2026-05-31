#include "mainWindow.h"
#include "GroupedTableView.h"
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto *table = new GroupedTableView(this);

    // Initialize the table with 20 rows and 10 columns
    table->init(20, 10);
    table->fillTestData();

    setCentralWidget(table);

    // Create demonstration grouping structure
    table->groupRow(4, 8);          // Button on row 5 collapses rows 6–9
    table->groupRow(9, 13);         // Button on row 10 collapses rows 11–14

    table->groupColumn(2, 5);       // Column C collapses columns D–F
    table->groupColumn(6, 8);       // Column G collapses columns H–I

    // --- Dynamic Window Sizing ---
    // Request the ideal sizing to view the entire table layout (all columns, rows, and headers).
    const QSize tableIdeal = table->sizeHint();

    // Query screen geometry to restrict window sizes on smaller displays
    const QScreen *screen  = QGuiApplication::primaryScreen();
    const QRect   screenGeo = screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);

    // Cap the maximum size to 2/3 of the display's height and width
    const int maxW = screenGeo.width()  * 2 / 3;
    const int maxH = screenGeo.height() * 2 / 3;

    // Accounts for window borders/decorations (4px padding offset)
    const int idealW = qMin(tableIdeal.width()  + 4, maxW);
    const int idealH = qMin(tableIdeal.height() + 4, maxH);

    resize(idealW, idealH);

    // Center the main window on the user's screen
    move(screenGeo.left() + (screenGeo.width()  - idealW) / 2,
         screenGeo.top()  + (screenGeo.height() - idealH) / 2);
}

MainWindow::~MainWindow() {}
