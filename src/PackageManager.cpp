#include "PackageManager.h"
#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include <QProcess>
#include <QDebug> // Для отладочных сообщений

PackageManager::PackageManager(QObject *parent) : QObject(parent) {
    loadPackageDefinitions();
}

PackageManager::~PackageManager() = default;

void PackageManager::loadPackageDefinitions() {
    // Здесь мы "хардкодим" определения пакетов для демонстрации.
    // В реальном приложении это можно загружать из XML, JSON или другого конфигурационного файла,
    // который также может быть частью ресурсов.

    availablePackages.append({
        "example_app1",
        "Пример Приложения 1 (Text File)",
        ":/packages/example_app1.tar.gz", // Путь в системе ресурсов
        "ExampleApp1"
    });

    availablePackages.append({
        "example_app2",
        "Пример Приложения 2 (Another Text)",
        ":/packages/example_app2.zip",   // Путь в системе ресурсов
        "ExampleApp2"
    });

    // Добавить сюда еще пакеты по необходимости
    // Например, для PostgreSQL (потребуется настоящий архив и команды):
    // availablePackages.append({
    //     "postgresql",
    //     "PostgreSQL Server",
    //     ":/packages/postgresql-13.3-linux-x64.tar.gz",
    //     "PostgreSQL13",
    //     // {"tar", "xzf", "%ARCHIVE_PATH%", "-C", "%TARGET_DIR%"} // Пример команды
    // });
    emit statusMessage("Определения пакетов загружены. Доступно: " + QString::number(availablePackages.size()));
}

QList<PackageInfo> PackageManager::getAvailablePackages() const {
    return availablePackages;
}

void PackageManager::installPackage(const PackageInfo& package) {
    emit installationStarted(package.displayName);
    emit statusMessage(QString("Начало установки: %1").arg(package.displayName));

    // 1. Проверяем, существует ли ресурс
    QFile resourceFile(package.resourcePath);
    if (!resourceFile.exists()) {
        QString errorMsg = QString("Ошибка: Ресурс для пакета '%1' не найден по пути '%2'")
                               .arg(package.displayName, package.resourcePath);
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        qWarning() << errorMsg;
        return;
    }

    // 2. Создаем временную директорию для извлечения архива
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        QString errorMsg = QString("Ошибка: Не удалось создать временную директорию для '%1'")
                               .arg(package.displayName);
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        qWarning() << errorMsg;
        return;
    }
    QString tempPath = tempDir.path();
    QString archiveFileName = QFileInfo(package.resourcePath).fileName();
    QString tempArchivePath = tempPath + QDir::separator() + archiveFileName;

    // 3. Копируем ресурс во временный файл
    if (!resourceFile.copy(tempArchivePath)) {
        QString errorMsg = QString("Ошибка: Не удалось скопировать ресурс '%1' во временный файл '%2'. Ошибка: %3")
                               .arg(package.resourcePath, tempArchivePath, resourceFile.errorString());
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        qWarning() << errorMsg;
        return;
    }
    emit statusMessage(QString("Ресурс '%1' скопирован в '%2'").arg(package.displayName, tempArchivePath));

    // 4. Определяем директорию для установки
    // Для демо можно использовать поддиректорию в QDir::homePath() или QDir::currentPath()
    QString installBaseDir = QDir::homePath() + QDir::separator() + "MyInstalledApps";
    QDir baseDir(installBaseDir);
    if (!baseDir.exists()) {
        if (!baseDir.mkpath(".")) {
             QString errorMsg = QString("Ошибка: Не удалось создать базовую директорию для установки: %1").arg(installBaseDir);
             emit statusMessage(errorMsg);
             emit installationFinished(package.displayName, false, errorMsg);
             qWarning() << errorMsg;
             return;
        }
    }
    QString targetInstallPath = installBaseDir + QDir::separator() + package.targetSubDir;
    QDir targetDir(targetInstallPath);
     if (!targetDir.exists()) {
        if (!targetDir.mkpath(".")) {
             QString errorMsg = QString("Ошибка: Не удалось создать целевую директорию для установки: %1").arg(targetInstallPath);
             emit statusMessage(errorMsg);
             emit installationFinished(package.displayName, false, errorMsg);
             qWarning() << errorMsg;
             return;
        }
    }


    // 5. Распаковка архива с помощью QProcess
    QProcess *unpacker = new QProcess(this);
    QString program;
    QStringList arguments;

    // Определяем команду в зависимости от типа архива (упрощенно)
    if (package.resourcePath.endsWith(".tar.gz")) {
        program = "tar"; // Убедитесь, что tar установлен в системе
        arguments << "xzf" << tempArchivePath << "-C" << targetInstallPath;
    } else if (package.resourcePath.endsWith(".zip")) {
        program = "unzip"; // Убедитесь, что unzip установлен в системе
        arguments << tempArchivePath << "-d" << targetInstallPath;
    } else {
        QString errorMsg = QString("Ошибка: Неподдерживаемый тип архива для '%1': %2")
                               .arg(package.displayName, package.resourcePath);
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        qWarning() << errorMsg;
        delete unpacker;
        return;
    }

    emit statusMessage(QString("Запуск распаковки: %1 %2").arg(program, arguments.join(" ")));

    // Асинхронный запуск процесса
    connect(unpacker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, package, unpacker, tempDir_path = tempDir.path()](int exitCode, QProcess::ExitStatus exitStatus) mutable {
        // tempDir будет удалена автоматически, когда объект tempDir выйдет из области видимости,
        // но мы скопировали ее путь, чтобы убедиться, что она не удалена до завершения процесса.
        // QTemporaryDir(tempDir_path); // Это создаст новый объект, который сразу удалится. Не нужно.

        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            QString successMsg = QString("Пакет '%1' успешно установлен в '%2'")
                                     .arg(package.displayName, QDir(QDir::homePath() + QDir::separator() + "MyInstalledApps" + QDir::separator() + package.targetSubDir).absolutePath());
            emit statusMessage(successMsg);
            emit installationFinished(package.displayName, true, successMsg);
            qInfo() << successMsg;
        } else {
            QString errorMsg = QString("Ошибка установки пакета '%1'. Код выхода: %2. Ошибка: %3. stderr: %4")
                                   .arg(package.displayName)
                                   .arg(exitCode)
                                   .arg(unpacker->errorString())
                                   .arg(QString::fromUtf8(unpacker->readAllStandardError()));
            emit statusMessage(errorMsg);
            emit installationFinished(package.displayName, false, errorMsg);
            qWarning() << errorMsg;
        }
        unpacker->deleteLater(); // Очищаем ресурсы QProcess
    });

    connect(unpacker, &QProcess::errorOccurred, this, [this, package, unpacker](QProcess::ProcessError error) {
        // Этот сигнал может сработать, если, например, программа (tar/unzip) не найдена
        if (unpacker->state() == QProcess::NotRunning) { // Если процесс даже не запустился
            QString errorMsg = QString("Ошибка запуска процесса распаковки для '%1'. Ошибка: %2")
                                .arg(package.displayName, unpacker->errorString());
            emit statusMessage(errorMsg);
            emit installationFinished(package.displayName, false, errorMsg);
            qWarning() << errorMsg;
            unpacker->deleteLater();
        }
    });
    
    connect(unpacker, &QProcess::readyReadStandardOutput, this, [this, unpacker](){
        emit statusMessage("STDOUT: " + QString::fromUtf8(unpacker->readAllStandardOutput()));
    });
    connect(unpacker, &QProcess::readyReadStandardError, this, [this, unpacker](){
        emit statusMessage("STDERR: " + QString::fromUtf8(unpacker->readAllStandardError()));
    });


    unpacker->start(program, arguments);

    if (!unpacker->waitForStarted(5000)) { // Таймаут для запуска
        QString errorMsg = QString("Процесс распаковки для '%1' не запустился вовремя. Программа: '%2'. Ошибка: %3")
                               .arg(package.displayName, program, unpacker->errorString());
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        qWarning() << errorMsg;
        unpacker->deleteLater(); // Очищаем
        return;
    }
}