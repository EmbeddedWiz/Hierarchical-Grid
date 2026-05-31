#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/**
 * @class MainWindow
 * @brief The main application window containing the hierarchical table view.
 *
 * This window instantiates the GroupedTableView, configures initial sample row
 * and column groups, populates dummy cell contents, and adjusts its own size 
 * dynamically based on screen geometry and table contents.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the MainWindow.
     * @param parent The parent widget.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for MainWindow.
     */
    ~MainWindow();
};

#endif // MAINWINDOW_H
