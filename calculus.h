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
    const char conjunction = '&';
    /*Символ, який відповідає за диз'юнкцію*/
    const char disjunction = 'V';
    /*Символ, який відповідає за заперечення*/
    const char denial = '!';
    /*Символ, який відповідає за початок виразу в дужках*/
    const char bracketsOpen = '(';
    /*Символ, який відповідає за кінець виразу в дужках*/
    const char bracketsClose = ')';

    /*Змінна для зберігання виразу, який необхідно обчислити*/
    QString expression;
    /*Змінна, яка зберігає кількість аргументів у виразі*/
    int numberVariables;
    /*Словник для зберегання номеру дії та її значення, яке є зрозумілим для комп'ютера. Деякі фрагменти замінені на цифри*/
    QMap<QString, QString> *table;
    /*Словник, який містить в собі текстовий варіант дії та її порядковий номер. Використовується для шапки таблиці*/
    QMap<int, QString> *headerTable;
    /*Словник для зберігання значень для кожного аргументу виразу*/
    QMap<QChar, QList<bool>> *valuesVariables;
    /*Словник для зберегання всіх результатів обчислення*/
    QList<QMap<int, bool>> *valuesExpression;
    /*Додатковий словник, який необхідний для того, щоб обчислити вираз та присвоїти масив даних номеру рядку в таблиці*/
    QMap<int, bool> *resultRow;

    /*Метод для визначення аргументів, створення таблиці з їх даними*/
    void calculateNumberVariables();
    /*Метод для пошуку елементів, які маю у своєму складі заперечення*/
    void denialPriority(QString &text);
    /*Метод для визначення виразі, які розміщенні в дужках та подальше розкладання їх на дії*/
    void bracketsPriority();
    /*Метод для опрацювання дій, які мають кон'юнкцію або диз'юнкцію*/
    void conjunctionOrDisjunctionPriority(QString &text, QChar element);
    /*Метод для перевірки чи вже було обчисленно вказану дію*/
    QString isDuplicate(QString &text);
    /*Метод для заповнення словника, який відповідає за шапку таблиці*/
    void createHeaderTable();
    /*Метод для окремих дій виразу*/
    void cleansing(QMap<QChar, bool> &leterValue);
    /*Метод для повернення булевого значення*/
    bool getValue(QString element, QMap<QChar, bool> &leterValue);
    QString chechSymbol(QString &text, int indexStart, Direction direction);
    QString findNumber(QString &text, int indexStart, Direction direction);

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
