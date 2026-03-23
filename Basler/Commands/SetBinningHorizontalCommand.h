#ifndef SETBINNINGHORIZONTALCOMMAND_H
#define SETBINNINGHORIZONTALCOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки коэффициента биннинга по горизонтали.
 *
 * При выполнении вызывает BaslerApi::applyBinningHorizontalChanging().
 */
class SetBinningHorizontalCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param binningHorizontal Новый коэффициент биннинга по горизонтали (1–4).
     */
    SetBinningHorizontalCommand(int binningHorizontal);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    int m_binningHorizontal; //!< Значение биннинга.
};

#endif // SETBINNINGHORIZONTALCOMMAND_H
