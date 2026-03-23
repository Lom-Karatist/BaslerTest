#ifndef SETHEIGHTCOMMAND_H
#define SETHEIGHTCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки высоты изображения.
 *
 * При выполнении вызывает BaslerApi::applyHeightChanging().
 */
class SetHeightCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param height Новая высота (пикселей).
     */
    SetHeightCommand(int height);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_height; //!< Высота изображения.
};

#endif // SETHEIGHTCOMMAND_H
