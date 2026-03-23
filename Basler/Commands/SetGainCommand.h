#ifndef SETGAINCOMMAND_H
#define SETGAINCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки усиления (Gain) камеры.
 *
 * При выполнении вызывает BaslerApi::applyGainChanging().
 */
class SetGainCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param gain Новое значение усиления.
     */
    explicit SetGainCommand(double gain);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    double m_gain; //!< Значение усиления.
};

#endif // SETGAINCOMMAND_H
