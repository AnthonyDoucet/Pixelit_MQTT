/**
 * @file main.cpp
 * @author Anthony Doucet
 * @version 1
*/

#include "mainwindow.h"

#include <QApplication>

/**
 * @brief Boucle Principale qMain
 * @details Permet de lancer MainWindow
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
