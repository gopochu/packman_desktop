#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStackedLayout>
#include <QWidget>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private slots:
    void on_nextButton_clicked();
    void on_backButton_clicked();

private:
    QLineEdit* surnameInput;
    QPushButton* backButton;
    QPushButton* nextButton;
    QFrame* centralFrame;
    QStackedLayout* stackedLayout;
    QWidget* registrationPage;
    QString surname;

    void connectSignals();
    void createPages();
    void setupCentralFrame();
    void setupButtons();
    void setupSurnameInput();
    void setupComboBox();
    QString getLineEditSurname();
};
#endif // MAINWINDOW_H
