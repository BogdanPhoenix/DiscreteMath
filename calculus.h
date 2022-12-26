#ifndef CALCULUS_H
#define CALCULUS_H

#include "qtablewidget.h"
#include <QString>
#include <QMap>
#include <QtMath>
#include <QRegularExpression>

class Calculus
{
private:
    enum Direction
    {
        Left,
        Rigth
    };

    /*Символ, який відповідає за кон'юнкцію*/
    const QChar conjunction = '&';
    /*Символ, який відповідає за диз'юнкцію*/
    const QChar disjunction = 'V';
    /*Символ, який відповідає за заперечення*/
    const QChar denial = '!';
    /*Символ, який відповідає за початок виразу в дужках*/
    const QChar bracketsOpen = '(';
    /*Символ, який відповідає за кінець виразу в дужках*/
    const QChar bracketsClose = ')';

    /*Змінна для зберігання виразу, який необхідно обчислити*/
    QString expression;
    /*Змінна, яка зберігає кількість аргументів у виразі*/
    int numberVariables;
    /*Словник для зберегання номеру дії та її значення, яке є зрозумілим для комп'ютера. Деякі фрагменти замінені на цифри*/
    QMap<int, QString> *table;
    /*Словник, який містить в собі текстовий варіант дії та її порядковий номер. Використовується для шапки таблиці*/
    QMap<int, QString> *headerTable;
    /*Словник для зберігання значень для кожного аргументу виразу*/
    QMap<QString, QList<bool>> *valuesVariables;
    /*Словник для зберегання всіх результатів обчислення*/
    QList<QMap<int, bool>> *valuesExpression;
    /*Додатковий словник, який необхідний для того, щоб обчислити вираз та присвоїти масив даних номеру рядку в таблиці*/
    QMap<int, bool> *resultRow;
    /*Змінна для зберігання значення регулярного виразу*/
    QString regularExpStr;
    /*Регулярний вираз*/
    QRegularExpression *regularExp;

    /*Метод для визначення аргументів, створення таблиці з їх даними*/
    void calculateNumberVariables();
    /*Метод для пошуку елементів, які маю у своєму складі заперечення*/
    void denialPriority(QString &text);
    /*Метод для визначення виразі, які розміщенні в дужках та подальше розкладання їх на дії*/
    void bracketsPriority();
    /*Метод для опрацювання дій, які мають кон'юнкцію або диз'юнкцію*/
    void conjunctionOrDisjunctionPriority(QString &text);
    /*Метод для перевірки чи вже було обчисленно вказану дію*/
    int isDuplicate(const QString &text);
    /*Метод для заповнення словника, який відповідає за шапку таблиці*/
    void createHeaderTable();
    /*Метод для окремих дій виразу*/
    void cleansing(QMap<QString, bool> &leterValue);
    /*Метод для повернення булевого значення*/
    bool getValue(QString element, QMap<QString, bool> &leterValue);
    /*Метод для обчислення виразу з для вказаної дії*/
    void actionsAbbreviation(QMap<int, QString> *orderActions, QString &text, const QChar &action);
    /*Метод для отримання значення аргументу*/
    QString getArgumentValue(const QString &argument);
    /*Метод для видалення дужок виразу*/
    QString deleteBrackets(const QString &text, const QChar &brackets);

public:
    /*Конструктор*/
    Calculus();
    /*Деструктор*/
    ~Calculus();
    /*Метод для отримання виразу*/
    void setExpression(QString text);
    /*Метод для обчислення вказаного виразу*/
    void priorityActions ();
    /*Метод для формування таблиці з результатами*/
    void fillingTable(QTableWidget &table);
};

#endif // CALCULUS_H
