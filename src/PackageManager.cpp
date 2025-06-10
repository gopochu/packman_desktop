#include "PackageManager.h"

#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include <QProcess>
#include <QDebug>
#include <QThread>
#include <QApplication>

PackageManager::PackageManager(QObject *parent) : QObject(parent) {
    loadPackageDefinitions();
}

PackageManager::~PackageManager() = default;

void PackageManager::loadPackageDefinitions() {
    // Определяем пакеты, используя короткие псевдонимы из .qrc
    availablePackages.append({
        "postgresql",
        "PostgreSQL 17.5 (Source)", // Отображаемое имя
        ":/packages/postgresql.tar.gz",
        "postgresql-17.5" // Имя папки для распаковки
    });

    availablePackages.append({
        "openldap",
        "OpenLDAP 2.6.10 (Source)",
        ":/packages/openldap.tgz",
        "openldap-2.6.10"
    });

    emit statusMessage("Определения пакетов загружены. Доступно: " + QString::number(availablePackages.size()));
}

QList<PackageInfo> PackageManager::getAvailablePackages() const {
    return availablePackages;
}

void PackageManager::installPackage(const PackageInfo& package) {
    emit installationStarted(package.displayName);
    emit statusMessage(QString("Начало установки: %1").arg(package.displayName));

    // 1. Проверка наличия ресурса
    QFile resourceFile(package.resourcePath);
    if (!resourceFile.exists() || resourceFile.size() == 0) {
        QString errorMsg = QString("КРИТИЧЕСКАЯ ОШИБКА: Ресурс '%1' не найден или пуст. Проверьте, что:\n1. Приложение запущено из папки сборки.\n2. Файл 'packages.rcc' существует и не пустой.\n3. Путь в коде ('%2') и alias в .qrc ('%3') совпадают.")
                               .arg(package.displayName)
                               .arg(package.resourcePath)
                               .arg(QFileInfo(package.resourcePath).fileName());
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        qWarning() << errorMsg;
        return;
    }

    // 2. Создание временной директории
    m_tempDir.reset(new QTemporaryDir);
    if (!m_tempDir->isValid()) {
        QString errorMsg = QString("Ошибка: Не удалось создать временную директорию.");
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        return;
    }

    QString tempPath = m_tempDir->path();
    QString archiveFileName = QFileInfo(package.resourcePath).fileName();
    QString tempArchivePath = tempPath + QDir::separator() + archiveFileName;

    // 3. Копирование архива
    emit statusMessage("Копирование архива во временную папку (может занять время)...");
    if (!resourceFile.copy(tempArchivePath)) {
        QString errorMsg = QString("Ошибка: Не удалось скопировать ресурс: %1").arg(resourceFile.errorString());
        emit statusMessage(errorMsg);
        emit installationFinished(package.displayName, false, errorMsg);
        return;
    }
    emit statusMessage(QString("Ресурс скопирован в '%1'").arg(tempArchivePath));

    // 4. Создание папки установки
    QString installBaseDir = QDir::homePath() + QDir::separator() + "MyInstalledApps";
    QString targetInstallPath = installBaseDir + QDir::separator() + package.targetSubDir;
    if (!QDir(installBaseDir).exists()) { QDir().mkpath(installBaseDir); }
    if (!QDir(targetInstallPath).exists()) { QDir().mkpath(targetInstallPath); }

    // 5. Распаковка архива
    QProcess *unpacker = new QProcess(this);
    QString program = "tar";
    QStringList arguments;
    arguments << "xzf" << tempArchivePath << "-C" << targetInstallPath;

    emit statusMessage(QString("Запуск распаковки: %1 %2").arg(program, arguments.join(" ")));

    connect(unpacker, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            QString successMsg = QString("Пакет '%1' успешно распакован в '%2'")
                                     .arg(package.displayName, targetInstallPath);
            emit statusMessage(successMsg);
            emit installationFinished(package.displayName, true, successMsg);
        } else {
            QString errorMsg = QString("Ошибка распаковки '%1'. stderr: %2")
                                   .arg(package.displayName, QString::fromUtf8(unpacker->readAllStandardError()));
            emit statusMessage(errorMsg);
            emit installationFinished(package.displayName, false, errorMsg);
        }

        unpacker->deleteLater();

        // Очистка временной директории после завершения установки
        m_tempDir.reset(nullptr);
    });

    unpacker->start(program, arguments);
}