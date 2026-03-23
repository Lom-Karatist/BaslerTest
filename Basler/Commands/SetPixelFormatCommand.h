#ifndef SETPIXELFORMATCOMMAND_H
#define SETPIXELFORMATCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки формата пикселя.
 *
 * При выполнении вызывает BaslerApi::applyPixelFormatChanging().
 */
class SetPixelFormatCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param pixelFormat Новый формат пикселя (значение из EPixelType).
     */
    explicit SetPixelFormatCommand(int pixelFormat);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_pixelFormat; //!< Значение формата пикселя.
};

#endif // SETPIXELFORMATCOMMAND_H
