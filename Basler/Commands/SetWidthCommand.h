#ifndef SETWIDTHCOMMAND_H
#define SETWIDTHCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки ширины изображения.
 *
 * При выполнении вызывает BaslerApi::applyWidthChanging().
 */
class SetWidthCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param width Новая ширина (пикселей).
     */
    explicit SetWidthCommand(int width);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_width; //!< Ширина изображения.
};

#endif // SETWIDTHCOMMAND_H
