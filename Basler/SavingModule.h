#ifndef SAVINGMODULE_H
#define SAVINGMODULE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QImage>
#include "Types.h"

/**
 * @class SavingModule
 * @brief Модуль для асинхронного сохранения данных от камер.
 *
 * Предоставляет методы для сохранения изображений в форматах BMP и Binary
 * (сырые данные). Сохранение выполняется в отдельном потоке с использованием
 * QtConcurrent, что не блокирует основной поток и поток захвата кадров.
 *
 * Управление форматом и путём сохранения осуществляется через методы setFormat()
 * и setSavingPath(). Сохранение активируется вызовом setIsNeedToSave(true).
 *
 * Класс является потомком QObject для возможности использования сигналов,
 * хотя в текущей реализации сигналы не используются.
 */
class SavingModule : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Конструктор.
     * @param parent Родительский QObject.
     */
    explicit SavingModule(QObject *parent = nullptr);

    /**
     * @brief Задать путь для сохранения файлов.
     * @param newSavingPath Полный путь к каталогу.
     *
     * Если каталог не существует, он будет создан.
     */
    void setSavingPath(const QString &newSavingPath);

    /**
     * @brief Задать формат сохраняемых файлов.
     * @param newFormat Формат сохранения (Bmp или Binary).
     */
    void setFormat(BaslerConstants::SavingFormat newFormat);

    /**
     * @brief Синхронное сохранение данных (используется для отладки или резервного варианта).
     * @param data Байтовый массив с данными.
     * @param width Ширина.
     * @param height Высота.
     * @param pixelFormat Формат пикселей.
     * @param appendix Префикс имени файла.
     * @param timeStamp Временная метка.
     *
     * Метод выполняет сохранение в текущем потоке. Рекомендуется использовать
     * асинхронную версию saveDataAsync() для production-кода.
     */
    void saveData(const QByteArray &data, int width, int height, int pixelFormat, QString appendix, QString timeStamp);

    /**
     * @brief Асинхронное сохранение данных (основной метод для использования).
     * @param data Байтовый массив с данными изображения.
     * @param width Ширина изображения в пикселях.
     * @param height Высота изображения.
     * @param pixelFormat Формат пикселей (значение из EPixelType).
     * @param prefix Префикс имени файла (например, "master" или "slave").
     * @param timeStamp Временная метка, используемая в имени файла.
     *
     * Метод определяет формат сохранения (через m_format) и запускает
     * асинхронную задачу через QtConcurrent. Вся операция выполняется в
     * отдельном потоке из глобального пула QThreadPool.
     */
    void saveDataAsync(const QByteArray &data, int width, int height, int pixelFormat,
                       const QString &prefix, const QString &timeStamp);

    /**
     * @brief Статический метод для асинхронного сохранения в формате BMP.
     * @param data Данные изображения.
     * @param width Ширина.
     * @param height Высота.
     * @param pixelFormat Формат пикселей.
     * @param prefix Префикс имени файла.
     * @param timeStamp Временная метка.
     * @param savingPath Путь к каталогу сохранения.
     *
     * Является точкой входа для QtConcurrent. Не зависит от экземпляра класса.
     */
    static void saveAsBmpAsync(const QByteArray &data, int width, int height, int pixelFormat,
                               const QString &prefix, const QString timeStamp,
                               const QString savingPath);

    /**
     * @brief Статический метод для асинхронного сохранения в бинарном формате.
     * @param data Данные изображения.
     * @param prefix Префикс имени файла.
     * @param timeStamp Временная метка.
     * @param savingPath Путь к каталогу сохранения.
     */
    static void saveAsBinaryAsync(const QByteArray &data, const QString &prefix,
                                  const QString timeStamp, const QString savingPath);

    /**
     * @brief Проверка, нужно ли сохранять данные.
     * @return true, если сохранение активно.
     */
    bool isNeedToSave() const;

    /**
     * @brief Включить/выключить сохранение.
     * @param newIsNeedToSave true – начать сохранять, false – остановить.
     */
    void setIsNeedToSave(bool newIsNeedToSave);

    /**
     * @brief Преобразовать сырые данные камеры в QImage.
     * @param data Байтовый массив.
     * @param width Ширина.
     * @param height Высота.
     * @param pixelFormat Формат пикселей.
     * @return QImage, скопированная из данных, или null-изображение в случае ошибки.
     *
     * Статический метод, используется как внутри класса, так и в CameraManager.
     */
    static QImage convertToQImage(const QByteArray &data, int width, int height, int pixelFormat);

private:
    /**
     * @brief Синхронное сохранение в BMP (вспомогательный метод).
     */
    void saveAsBmp(const QByteArray &data, int width, int height, int pixelFormat, const QString &prefix, QString timeStamp);

    /**
     * @brief Синхронное сохранение в бинарном формате.
     */
    void saveAsBinary(const QByteArray &data, const QString &prefix, QString timeStamp);

    /**
     * @brief Сгенерировать полное имя файла (без расширения).
     * @param prefix Префикс.
     * @param timeStamp Временная метка.
     * @return Полный путь вида "path/prefix_timestamp".
     */
    QString generateFileName(const QString &prefix, QString timeStamp) const;

    bool m_isNeedToSave;                        //!< Флаг: сохранять данные или нет.
    QString m_savingPath;                       //!< Путь к каталогу для сохранения.
    BaslerConstants::SavingFormat m_format;     //!< Текущий формат сохранения.
};

#endif // SAVINGMODULE_H
