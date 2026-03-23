#ifndef SETOFFSETYCOMMAND_H
#define SETOFFSETYCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки смещения по вертикали (OffsetY).
 *
 * При выполнении вызывает BaslerApi::applyOffsetYChanging().
 */
class SetOffsetYCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param offsetY Новое смещение по Y (пикселей).
     */
    SetOffsetYCommand(int offsetY);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_offsetY; //!< Смещение по Y.
};

#endif // SETOFFSETYCOMMAND_H
