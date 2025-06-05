#pragma once

#include <QString>
#include <QStringList>

// Структура для хранения информации о пакете
struct PackageInfo {
    QString id;                 // Уникальный идентификатор пакета
    QString displayName;        // Имя для отображения в ComboBox
    QString resourcePath;       // Путь к архиву пакета в системе ресурсов Qt (например, ":/packages/my_package.tar.gz")
    QString targetSubDir;       // Имя поддиректории для установки (например, "my_package_files")
    // QStringList extractionCommand; // Можно добавить, если команды распаковки разные для разных архивов
                                   // Например: {"tar", "xf", "%ARCHIVE_PATH%", "-C", "%TARGET_DIR%"}
                                   // или {"unzip", "%ARCHIVE_PATH%", "-d", "%TARGET_DIR%"}
};