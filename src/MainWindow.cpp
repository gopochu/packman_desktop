#include "MainWindow.h"

#include <QObject>
#include <QStackedLayout>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QMessageBox>
#include <fmt/format.h>
#include <QComboBox>

MainWindow::MainWindow() : QMainWindow(nullptr) {
    setupCentralFrame();
    createPages();
    setupSurnameInput();
    setupButtons();
    connectSignals();
    stackedLayout->setCurrentIndex(0);
}

MainWindow::~MainWindow() = default;

void MainWindow::on_nextButton_clicked() {
    if (stackedLayout) stackedLayout->setCurrentIndex(1);
}

void MainWindow::on_backButton_clicked() {
    if (stackedLayout) stackedLayout->setCurrentIndex(0);
}

void MainWindow::connectSignals() {
    // Подключение кнопки "Далее"
    connect(nextButton, &QPushButton::clicked, this, [this]() {
        surname = surnameInput->text().trimmed();
        
        if(surname.isEmpty()) {
            QMessageBox::critical(this, "Ошибка", "Поле не может быть пустым!");
            surnameInput->setFocus();
            return;
        }

        // Переходим на следующую страницу
        stackedLayout->setCurrentIndex(1);
    
        fmt::print("Фамилия сохранена: {}\n", surname.toStdString());
    });

    connect(backButton, &QPushButton::clicked, this, &MainWindow::on_backButton_clicked);
}

// Метод для настройки центрального фрейма
void MainWindow::setupCentralFrame() {
    centralFrame = new QFrame(this);
    centralFrame->setFrameShape(QFrame::StyledPanel);
    setCentralWidget(centralFrame);
    
    stackedLayout = new QStackedLayout(centralFrame);
    centralFrame->setLayout(stackedLayout);
}

// Метод для создания страниц
void MainWindow::createPages() {
    // Явно создаем и сохраняем страницы
    registrationPage = new QWidget(centralFrame);
    QWidget* choosePage = new QWidget(centralFrame);
    
    registrationPage->setStyleSheet("background-color: lightblue;");
    choosePage->setStyleSheet("background-color: lightgreen;");
    
    stackedLayout->addWidget(registrationPage);
    stackedLayout->addWidget(choosePage);
}

void MainWindow::setupSurnameInput() {
    // Основной layout страницы регистрации
    QVBoxLayout* pageLayout = new QVBoxLayout(registrationPage);
    pageLayout->setContentsMargins(50, 50, 50, 50); // Отступы
    
    // 1. Верхняя часть (растягивается)
    QSpacerItem* topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    pageLayout->addItem(topSpacer);
    
    // 2. Центральная часть с QLineEdit
    QHBoxLayout* centerLayout = new QHBoxLayout();
    
    // Левое растяжение
    centerLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    // Поле ввода
    surnameInput = new QLineEdit(registrationPage);
    surnameInput->setPlaceholderText("Введите вашу фамилию");
    surnameInput->setFixedSize(300, 30); // Фиксированный размер
    centerLayout->addWidget(surnameInput);
    
    // Правое растяжение
    centerLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    
    pageLayout->addLayout(centerLayout);
    
    // 3. Нижняя часть (растягивается + кнопки)
    QSpacerItem* middleSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    pageLayout->addItem(middleSpacer);
}

void MainWindow::setupButtons() {
    // Создаем контейнер для кнопок
    QWidget* buttonContainer = new QWidget(registrationPage);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonContainer);
    
    // Настраиваем кнопки
    backButton = new QPushButton("Назад", buttonContainer);
    nextButton = new QPushButton("Далее", buttonContainer);
    
    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(nextButton);
    
    // Добавляем контейнер с кнопками на страницу
    QVBoxLayout* pageLayout = qobject_cast<QVBoxLayout*>(registrationPage->layout());
    pageLayout->addWidget(buttonContainer);
}

QString MainWindow::getLineEditSurname() {
    return surnameInput->text();
}