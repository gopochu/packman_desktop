#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedLayout>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>


#include "PackageManager.h"
#include "PackageInfo.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleNextButton();
    void handleBackButton();
    void updateButtonStates(int pageIndex);
    void handleInstallationStatus(const QString& packageName, bool success, const QString& message);
    void displayStatusMessage(const QString& message);

private:
    //UI элементы для страниц
    QLineEdit* surnameInput;
    QComboBox* packageComboBox;
    QLabel* statusLabel;

    //Основные UI элементы окна
    QPushButton* backButton;
    QPushButton* nextButton;
    QFrame* centralFrame;
    QStackedLayout* stackedLayout;

    //Страницы
    QWidget* pageSurname;
    QWidget* pagePacketSelect;
    QWidget* registrationPage;

    //Переменные для хранения данных
    QString currentSurname;
    PackageManager* packageManager;

    void setupUI();
    void createPages();
    void setupPageSurname();
    void setupPagePackageSelect();
    void connectSignalsAndSlots();
    void loadPackagesToComboBox();
};
#endif // MAINWINDOW_H
