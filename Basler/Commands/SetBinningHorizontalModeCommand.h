#ifndef SETBINNINGHORIZONTALMODECOMMAND_H
#define SETBINNINGHORIZONTALMODECOMMAND_H

#include "Basler/BaslerApi.h"
#include "ParameterCommand.h"

/**
 * @brief Команда для установки режима биннинга по горизонтали.
 *
 * При выполнении вызывает BaslerApi::applyBinningHorizontalModeChanging().
 */
class SetBinningHorizontalModeCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param mode Новый режим биннинга (Sum или Average).
     */
    explicit SetBinningHorizontalModeCommand(BinningHorizontalModeEnums mode);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    BinningHorizontalModeEnums m_mode; //!< Режим биннинга.
};

#endif // SETBINNINGHORIZONTALMODECOMMAND_H
