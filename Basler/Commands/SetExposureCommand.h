#ifndef SETEXPOSURECOMMAND_H
#define SETEXPOSURECOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки экспозиции камеры.
 *
 * При выполнении вызывает BaslerApi::applyExposureChanging().
 */
class SetExposureCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param exposureMs Новая экспозиция в миллисекундах.
     */
    explicit SetExposureCommand(double exposureMs);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    double m_exposureMs; //!< Значение экспозиции (мс).
};

#endif // SETEXPOSURECOMMAND_H
