#ifndef SETBINNINGVERTICALMODECOMMAND_H
#define SETBINNINGVERTICALMODECOMMAND_H

#include "Basler/BaslerApi.h"
#include "ParameterCommand.h"

/**
 * @brief Команда для установки режима биннинга по вертикали.
 *
 * При выполнении вызывает BaslerApi::applyBinningVerticalModeChanging().
 */
class SetBinningVerticalModeCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param mode Новый режим биннинга (Sum или Average).
     */
    explicit SetBinningVerticalModeCommand(BinningVerticalModeEnums mode);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    BinningVerticalModeEnums m_mode; //!< Режим биннинга.
};

#endif // SETBINNINGVERTICALMODECOMMAND_H
