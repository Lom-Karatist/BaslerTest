#ifndef SETOFFSETXCOMMAND_H
#define SETOFFSETXCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки смещения по горизонтали (OffsetX).
 *
 * При выполнении вызывает BaslerApi::applyOffsetXChanging().
 */
class SetOffsetXCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param offsetX Новое смещение по X (пикселей).
     */
    SetOffsetXCommand(int offsetX);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_offsetX; //!< Смещение по X.
};

#endif // SETOFFSETXCOMMAND_H
