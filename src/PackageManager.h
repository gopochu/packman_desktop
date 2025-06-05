#pragma once

#include <QObject>
#include <QList>
#include "PackageInfo.h"

class PackageManager : public QObject {
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = nullptr);
    ~PackageManager();

    // Возвращает список доступных для установки пакетов
    QList<PackageInfo> getAvailablePackages() const;

    // Начинает установку выбранного пакета
    void installPackage(const PackageInfo& package);

signals:
    // Сигнал о начале процесса установки
    void installationStarted(const QString& packageName);
    // Сигнал о завершении установки (true - успех, false - ошибка)
    void installationFinished(const QString& packageName, bool success, const QString& message);
    // Сигнал для вывода сообщений в лог или статусную строку
    void statusMessage(const QString& message);


private:
    QList<PackageInfo> availablePackages; // Список пакетов, известных менеджеру

    // Инициализация списка доступных пакетов (можно загружать из конфиг. файла или захардкодить)
    void loadPackageDefinitions();
};