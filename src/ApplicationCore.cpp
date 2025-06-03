#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(800, 600);
    w.setWindowTitle("Stacked Layout Example");
    w.show();
    return a.exec();
}