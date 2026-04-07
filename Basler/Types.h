#ifndef TYPES_H
#define TYPES_H

#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/PylonIncludes.h>

#include <QString>
#include <QStringList>

using namespace Pylon;
using namespace Basler_UniversalCameraParams;

/**
 * @brief Структура для хранения параметров камеры, загружаемых из INI-файла.
 */
struct BaslerCameraParams {
    QString
        serialNumber;  //!< Серийный номер камеры (уникальный идентификатор).
    bool isMaster;  //!< Флаг роли камеры: true — мастер, false — слейв.

    double exposureTime;  //!< Время экспозиции в миллисекундах.
    double gain;  //!< Усиление (gain) в условных единицах камеры.
    double acquisitionFrameRate;  //!< Желаемая частота кадров (только для
                                  //!< мастера).
    int pixelFormat;  //!< Формат пикселей (например, PixelType_Mono8).

    int offsetX;  //!< Смещение по X для выделения области интереса на матрице
    int offsetY;  //!< Смещение по Y для выделения области интереса на матрице
    int width;  //!< Ширина захватываемого изображения в пикселях.
    int height;  //!< Высота захватываемого изображения в пикселях.
    int binningHorizontal;  //!< Биннинг по горизонтали (от 1 до 4)
    int binningVertical;  //!< Биннинг по вертикали (от 1 до 4)
    BinningHorizontalModeEnums
        binningHorizontalMode;  //!< Режим биннинга по горизонтали
    BinningVerticalModeEnums
        binningVerticalMode;  //!< Режим биннинга по вертикали
};

/**
 * @namespace BaslerConstants
 * @brief Содержит константные списки для отображения в GUI и преобразования
 * строк.
 *
 * Функции возвращают предопределённые списки значений для форматов пикселей,
 * коэффициентов биннинга и режимов биннинга. Используются в формах настроек
 * и при загрузке/сохранении параметров.
 */
namespace BaslerConstants {
/**
 * @brief Список названий форматов пикселей.
 * @return QStringList {"Mono8", "Mono12", "Mono12p"}
 */
inline const QStringList pixelFormats() {
    static const QStringList list = {"Mono8", "Mono12", "Mono12p"};
    return list;
}

/**
 * @brief Список возможных коэффициентов биннинга.
 * @return QStringList {"1", "2", "3", "4"}
 */
inline const QStringList binningTypes() {
    static const QStringList list = {"1", "2", "3", "4"};
    return list;
}

/**
 * @brief Список режимов биннинга.
 * @return QStringList {"Sum", "Average"}
 */
inline const QStringList binningModes() {
    static const QStringList list = {"Sum", "Average"};
    return list;
}

/**
 * @enum SettingTypes
 * @brief Перечисление всех типов настроек, которые могут изменяться из внешних
 * модулей.
 *
 * Используется в сигнале `settingsWereChanged` для идентификации изменённого
 * параметра.
 */
enum SettingTypes {
    Exposure,     //!< Экспозиция (микросекунды)
    Gain,         //!< Усиление (Gain)
    PixelFormat,  //!< Формат пикселя (Mono8, Mono12, Mono12p)
    AcquisitionFramerate,  //!< Частота кадров (FPS)
    Width,                 //!< Ширина изображения
    Height,                //!< Высота изображения
    OffsetX,               //!< Смещение по X
    OffsetY,               //!< Смещение по Y
    BinningHorizontal,  //!< Коэффициент биннинга по горизонтали
    BinningVertical,  //!< Коэффициент биннинга по вертикали
    BinningHorizontalMode,  //!< Режим биннинга по горизонтали (Sum/Average)
    BinningVerticalMode,  //!< Режим биннинга по вертикали (Sum/Average)

    SizeAny,  //!< Размер по любой из осей (не для взаимодействия через API)
    OffsetAny,  //!< Смещение по любой из осей (не для взаимодействия через API)
    BinningAny,  //!< Биннинг по любой из осей (не для взаимодействия через API)
};

/**
 * @enum SavingFormat
 * @brief Перечисление всех типов сохранения данных камер Basler
 */
enum SavingFormat {
    Bmp,    //!< Изображения в формате BMP
    Binary  //!< Бинарные файлы
};
}  // namespace BaslerConstants

#endif  // TYPES_H
