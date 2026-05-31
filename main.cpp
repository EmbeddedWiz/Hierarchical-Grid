/**
 * @file main.cpp
 * @brief Entry point for the Hierarchical-Grid application.
 */

#include "mainWindow.h"
#include <QApplication>

/**
 * @brief Main entry point.
 * @return Application exit code.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
