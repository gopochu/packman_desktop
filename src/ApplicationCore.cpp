#include "MainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QResource>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QString resourcePath = QCoreApplication::applicationDirPath() + "/packages.rcc";

    MainWindow w;
    w.resize(800, 600);
    w.setWindowTitle("Stacked Layout Example");
    w.show();
    return a.exec();
}