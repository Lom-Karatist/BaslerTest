#ifndef SETFRAMERATECOMMAND_H
#define SETFRAMERATECOMMAND_H

#include "ParameterCommand.h"

/**
 * @brief Команда для установки частоты кадров камеры (только для мастера).
 *
 * При выполнении вызывает BaslerApi::applyFramerateChanging().
 */
class SetFramerateCommand : public ParameterCommand
{
public:
    /**
     * @brief Конструктор.
     * @param fps Новая частота кадров (кадров/с).
     */
    explicit SetFramerateCommand(double fps);

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi.
     */
    void execute(BaslerApi* api) override;

private:
    double m_fps; //!< Значение частоты кадров.
};

#endif // SETFRAMERATECOMMAND_H
