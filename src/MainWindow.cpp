#include "MainWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    packageManager = new PackageManager(this);
    setupUI();
    connectSignalsAndSlots();

    // Начальное состояние: первая страница, кнопки настроены
    stackedLayout->setCurrentIndex(0);
    updateButtonStates(0); // Устанавливаем состояние кнопок для первой страницы
    loadPackagesToComboBox(); // Загружаем пакеты в ComboBox
}

MainWindow::~MainWindow() {
    // packageManager удалится автоматически, т.к. MainWindow его родитель
}

void MainWindow::setupUI() {
    // Центральный виджет и основной layout
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    setCentralWidget(mainWidget);

    // Фрейм для страниц
    centralFrame = new QFrame(this);
    centralFrame->setFrameShape(QFrame::StyledPanel);
    stackedLayout = new QStackedLayout(centralFrame); // QStackedLayout теперь внутри centralFrame

    createPages(); // Создаем и добавляем страницы в stackedLayout

    // Контейнер для кнопок навигации
    QWidget *buttonContainer = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);

    backButton = new QPushButton("Назад", buttonContainer);
    nextButton = new QPushButton("Далее", buttonContainer);

    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch(1); // Растягиваем пространство между кнопками
    buttonLayout->addWidget(nextButton);

    // Добавляем элементы в главный layout
    mainLayout->addWidget(centralFrame); // centralFrame займет большую часть места
    mainLayout->addWidget(buttonContainer); // Кнопки внизу

    // Начальное состояние кнопок
    backButton->setVisible(false);
}

void MainWindow::createPages() {
    // Страница 1: Ввод фамилии
    pageSurname = new QWidget(centralFrame);
    setupPageSurname(); // Настраиваем содержимое этой страницы
    stackedLayout->addWidget(pageSurname);

    // Страница 2: Выбор пакета
    pagePacketSelect = new QWidget(centralFrame);
    setupPagePackageSelect(); // Настраиваем содержимое этой страницы
    stackedLayout->addWidget(pagePacketSelect);
}

void MainWindow::setupPageSurname() {
    QVBoxLayout* pageLayout = new QVBoxLayout(pageSurname);
    pageLayout->setContentsMargins(50, 50, 50, 50);
    pageLayout->setAlignment(Qt::AlignCenter); // Центрируем содержимое

    // Верхний спейсер
    pageLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Горизонтальный layout для центрирования поля ввода
    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)); // Левый спейсер

    surnameInput = new QLineEdit(pageSurname);
    surnameInput->setPlaceholderText("Введите вашу фамилию");
    surnameInput->setFixedSize(300, 30);
    inputLayout->addWidget(surnameInput);

    inputLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)); // Правый спейсер
    pageLayout->addLayout(inputLayout);

    // Нижний спейсер
    pageLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Для отладки
    pageSurname->setStyleSheet("background-color: lightblue;");
}

void MainWindow::setupPagePackageSelect() {
    QVBoxLayout* pageLayout = new QVBoxLayout(pagePacketSelect);
    pageLayout->setContentsMargins(20, 20, 20, 20);
    pageLayout->setAlignment(Qt::AlignTop); // Выравнивание по верху

    QLabel* instructionLabel = new QLabel("Выберите программу для установки:", pagePacketSelect);
    pageLayout->addWidget(instructionLabel);

    packageComboBox = new QComboBox(pagePacketSelect);
    pageLayout->addWidget(packageComboBox);

    // Место для отображения статуса установки
    statusLabel = new QLabel("Статус: Ожидание выбора...", pagePacketSelect);
    statusLabel->setWordWrap(true);
    pageLayout->addWidget(statusLabel);
    
    pageLayout->addStretch(1); // Чтобы элементы не растягивались на всю высоту

    // Для отладки
    pagePacketSelect->setStyleSheet("background-color: lightgreen;");
}

void MainWindow::loadPackagesToComboBox() {
    packageComboBox->clear();
    QList<PackageInfo> packages = packageManager->getAvailablePackages();
    if (packages.isEmpty()) {
        packageComboBox->addItem("Нет доступных пакетов");
        packageComboBox->setEnabled(false);
        qDebug() << "Список пакетов пуст.";
    } else {
        for (const PackageInfo& pkg : packages) {
            packageComboBox->addItem(pkg.displayName, QVariant::fromValue(pkg.id)); // Сохраняем ID пакета
        }
        packageComboBox->setEnabled(true);
        qDebug() << "Загружено пакетов в ComboBox:" << packages.size();
    }
}


void MainWindow::connectSignalsAndSlots() {
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::handleNextButton);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::handleBackButton);
    connect(stackedLayout, &QStackedLayout::currentChanged, this, &MainWindow::updateButtonStates);

    // Подключение к сигналам PackageManager
    connect(packageManager, &PackageManager::installationStarted, this, [this](const QString& packageName){
        statusLabel->setText(QString("Началась установка %1...").arg(packageName));
        nextButton->setEnabled(false); // Блокируем кнопку "Установить" во время установки
        backButton->setEnabled(false); // И кнопку "Назад"
    });
    connect(packageManager, &PackageManager::installationFinished, this, &MainWindow::handleInstallationStatus);
    connect(packageManager, &PackageManager::statusMessage, this, &MainWindow::displayStatusMessage);
}

void MainWindow::handleNextButton() {
    int currentIndex = stackedLayout->currentIndex();

    if (currentIndex == 0) { // Страница ввода фамилии
        currentSurname = surnameInput->text().trimmed();
        if (currentSurname.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Поле 'Фамилия' не может быть пустым!");
            surnameInput->setFocus();
            return;
        }
        qDebug() << "Фамилия сохранена:" << currentSurname.toStdString().c_str(); // Используем c_str() для fmt если он нужен
        stackedLayout->setCurrentIndex(1);
    } else if (currentIndex == 1) { // Страница выбора пакета (кнопка "Установить")
        if (packageComboBox->currentIndex() < 0 || !packageComboBox->isEnabled()) {
            QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите пакет для установки.");
            return;
        }
        QString selectedPackageId = packageComboBox->currentData().toString();
        QList<PackageInfo> packages = packageManager->getAvailablePackages();
        PackageInfo packageToInstall;
        bool found = false;
        for(const auto& pkg : packages) {
            if (pkg.id == selectedPackageId) {
                packageToInstall = pkg;
                found = true;
                break;
            }
        }

        if (found) {
            qDebug() << "Запрос на установку пакета:" << packageToInstall.displayName;
            statusLabel->setText(QString("Подготовка к установке %1...").arg(packageToInstall.displayName));
            packageManager->installPackage(packageToInstall);
        } else {
             QMessageBox::critical(this, "Ошибка", "Выбранный пакет не найден.");
             statusLabel->setText("Ошибка: Выбранный пакет не найден.");
        }
    }
}

void MainWindow::handleBackButton() {
    int currentIndex = stackedLayout->currentIndex();
    if (currentIndex > 0) {
        stackedLayout->setCurrentIndex(currentIndex - 1);
        statusLabel->setText("Статус: Ожидание выбора..."); // Сбрасываем статус при возврате
    }
}

void MainWindow::updateButtonStates(int pageIndex) {
    if (pageIndex == 0) { // Страница ввода фамилии
        nextButton->setText("Далее");
        nextButton->setEnabled(true);
        backButton->setVisible(false);
        surnameInput->setFocus(); // Устанавливаем фокус на поле ввода
    } else if (pageIndex == 1) { // Страница выбора пакета
        nextButton->setText("Установить");
        nextButton->setEnabled(packageComboBox->count() > 0 && packageComboBox->isEnabled()); // Активна, если есть пакеты
        backButton->setVisible(true);
        backButton->setEnabled(true); // Всегда активна на второй странице (если не идет установка)
    }
}

void MainWindow::handleInstallationStatus(const QString& packageName, bool success, const QString& message) {
    Q_UNUSED(packageName); // packageName может быть полезен для более детального лога
    
    statusLabel->setText(message);
    if (success) {
        QMessageBox::information(this, "Установка завершена", message);
    } else {
        QMessageBox::critical(this, "Ошибка установки", message);
    }
    // Разблокируем кнопки после завершения установки (успешной или нет)
    nextButton->setEnabled(true);
    backButton->setEnabled(true);
    updateButtonStates(stackedLayout->currentIndex()); // Обновляем состояние кнопок по текущей странице
}

void MainWindow::displayStatusMessage(const QString& message) {
    // Этот слот можно использовать для вывода более подробных логов в statusLabel
    // или в отдельный QTextEdit для логов, если потребуется.
    // Пока просто выводим в qDebug и, возможно, обновляем statusLabel, если это не сообщение о финальном результате.
    qDebug() << "PackageManager Status:" << message;
}