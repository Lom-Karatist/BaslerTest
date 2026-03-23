#ifndef PARAMETERCOMMAND_H
#define PARAMETERCOMMAND_H

class BaslerApi;

/**
 * @brief Абстрактный базовый класс для всех команд изменения параметров камеры.
 *
 * Реализует паттерн "Команда". Каждая конкретная команда инкапсулирует
 * запрос на изменение одного параметра. Команды могут быть собраны в список
 * и выполнены последовательно в потоке камеры с остановкой захвата.
 */
class ParameterCommand
{
public:
    /// Виртуальный деструктор.
    virtual ~ParameterCommand() = default;

    /**
     * @brief Выполнить команду.
     * @param api Указатель на объект BaslerApi, к которому применяется команда.
     *
     * Реализация должна вызывать соответствующий метод apply*Changing у BaslerApi.
     */
    virtual void execute(BaslerApi* api) = 0;
};

#endif // PARAMETERCOMMAND_H
