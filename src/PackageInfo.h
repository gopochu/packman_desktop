#pragma once

#include <QString>
#include <QStringList>

// Структура для хранения информации о пакете
struct PackageInfo {
    QString id; // Уникальный идентификатор пакета
    QString displayName; // Отображаемое имя
    QString resourcePath; // Путь к архиву пакета в системе ресурсов Qt (например, ":/packages/my_package.tar.gz")
    QString targetSubDir; // Имя папки для распаковки
};