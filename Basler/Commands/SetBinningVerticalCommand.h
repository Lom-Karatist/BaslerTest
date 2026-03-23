#ifndef SETBINNINGVERTICALCOMMAND_H
#define SETBINNINGVERTICALCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки коэффициента биннинга по вертикали.
 *
 * При выполнении вызывает BaslerApi::applyBinningVerticalChanging().
 */
class SetBinningVerticalCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param binningVertical Новый коэффициент биннинга по вертикали (1–4).
     */
    SetBinningVerticalCommand(int binningVertical);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_binningVertical; //!< Значение биннинга.
};

#endif // SETBINNINGVERTICALCOMMAND_H
