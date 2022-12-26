#include "calculus.h"
#include <QMessageBox>

/*Конструктор*/
Calculus::Calculus()
{
    table = new QMap<int, QString>();
    headerTable = new QMap<int, QString>();
    valuesVariables = new QMap<QString, QList<bool>>();
    valuesExpression = new QList<QMap<int, bool>>();

    regularExpStr.append(conjunction);
    regularExpStr.append(disjunction);
    regularExp = new QRegularExpression("[" + regularExpStr + "]");
}
/*Деструктор*/
Calculus::~Calculus()
{
    delete regularExp;
    delete table;
    delete headerTable;
    delete valuesVariables;
    delete valuesExpression;
    if(!resultRow->isEmpty()){
        delete resultRow;
    }
}
/*Метод для отримання виразу*/
void Calculus::setExpression(QString text)
{
    if(text.isEmpty())
    {
        QMessageBox::warning(0, "Увага", "Ви не ввели вираз для обчислення. Будь ласка введіть вираз, щоб продовжити роботу.");
        return;
    }
    this->expression = text;
}
/*Метод для обчислення вказаного виразу*/
void Calculus::priorityActions()
{
    QMap<QString, bool> *result;
    //Видалення всіх пробілів
    expression = expression.simplified();
    expression.replace(" ", "");

    table->clear();
    headerTable->clear();
    for(QMap<QString, QList<bool>>::iterator i = valuesVariables->begin(); i != valuesVariables->end(); ++i)
    {
        i.value().clear();
    }
    valuesVariables->clear();
    for(QList<QMap<int, bool>>::iterator i = valuesExpression->begin(); i != valuesExpression->end(); ++i)
    {
        i->clear();
    }
    valuesExpression->clear();

    calculateNumberVariables();
    denialPriority(expression);
    bracketsPriority();
    conjunctionOrDisjunctionPriority(expression);
    createHeaderTable();

    for(int i = 0; i < qPow(2, numberVariables); ++i)
    {
        result = new QMap<QString, bool>();
        resultRow = new QMap<int, bool>();
        //Формування фрагменту значень елементів, які будуть задіюватися для обчислення
        for(QMap<QString, QList<bool>>::iterator it = valuesVariables->begin(); it != valuesVariables->end(); ++it)
        {
            result->insert(it.key(), it.value().at(i));
        }
        cleansing(*result);
        valuesExpression->push_back(*resultRow);
        delete result;
    }
}
/*Метод для формування таблиці з результатами*/
void Calculus::fillingTable(QTableWidget &tableWork)
{
    int index;
    QMap<QString, QList<bool>>::iterator it_Variables = valuesVariables->begin();
    QList<QMap<int, bool>>::iterator it_Expression = valuesExpression->begin();
    QMap<int, QString>::iterator it_Header = headerTable->begin();

    //Оновлення даних таблиці
    for(int i = 0; i < tableWork.columnCount(); ++i){
        tableWork.removeColumn(i);
    }
    tableWork.setRowCount(qPow(2, numberVariables));
    tableWork.setColumnCount(valuesVariables->count() + headerTable->count());

    //Заповнення шапки таблиці
    for(int i = 0; i < tableWork.columnCount(); ++i){
        if(i < valuesVariables->count()){
            tableWork.setHorizontalHeaderItem(i, new QTableWidgetItem(it_Variables.key()));
            ++it_Variables;
        }
        else{
            tableWork.setHorizontalHeaderItem(i, new QTableWidgetItem(it_Header.value()));
            ++it_Header;
        }
    }

    //Заповнення таблиці значеннями
    for(int i = 0; i < tableWork.rowCount(); ++i){
        index = 0;
        for(it_Variables = valuesVariables->begin(); it_Variables != valuesVariables->end(); ++it_Variables)
        {
            tableWork.setItem(i, index++, new QTableWidgetItem(QString::number(it_Variables.value().at(i))));
        }
        for(QMap<int, bool>::iterator its = it_Expression->begin(); its != it_Expression->end(); ++its)
        {
            tableWork.setItem(i, index++, new QTableWidgetItem(QString::number(its.value())));
        }
        ++it_Expression;
    }
}
/*Метод для визначення аргументів, створення таблиці з їх даними*/
void Calculus::calculateNumberVariables()
{
    bool push;
    QList<bool> *listData;
    QString regularString;
    regularString.append(conjunction);
    regularString.append(disjunction);
    regularString.append(bracketsOpen);
    regularString.append(bracketsClose);
    regularString.append(denial);
    QRegularExpression *regular = new QRegularExpression("[" + regularString + "]");
    QStringList list = expression.split(*regular);
    list.removeDuplicates();
    list.removeOne("");
    list.sort();
    numberVariables = list.count();
    int num = numberVariables - 1;

    for(int i = 0; i < list.count(); ++i)
    {
        push = false;
        listData = new QList<bool>();
        for(int j = 0; j < numberVariables * numberVariables; ++j)
        {
            if(j % (int)qPow(2, num) == 0)
            {
                push = !push;
            }
            listData->push_back(push);
        }
        valuesVariables->insert(list.at(i), *listData);
        --num;
        delete listData;
    }

    delete regular;
}
/*Метод для пошуку елементів, які маю у своєму складі заперечення*/
void Calculus::denialPriority(QString &text)
{
    int index;
    QString regularString;
    regularString.append(conjunction);
    regularString.append(disjunction);
    regularString.append(bracketsOpen);
    regularString.append(bracketsClose);
    QRegularExpression *regular = new QRegularExpression("[" + regularString + "]");
    QStringList list = text.split(*regular);
    list.removeDuplicates();

    foreach (QString value, list)
    {
        if(!value.isEmpty() && value.at(0) == denial && value.length() > 1)
        {
            index = isDuplicate(value);
            if(index == -1)
            {
                index = table->size();
                table->insert(index, value);
            }
            text = text.replace(value, "*" + QString::number(index) + "*");
        }
    }
    delete regular;
}
/*Метод для визначення виразі, які розміщенні в дужках та подальше розкладання їх на дії*/
void Calculus::bracketsPriority()
{
    QString result;
    QString text;
    for(int i = 0; i < expression.length() - 1; ++i)
    {
        if(expression[i] == bracketsOpen){
            for(int j = i + 1; j < expression.length(); ++j)
            {
                if(expression[j] == bracketsOpen)
                {
                    i = j;
                }
                else if(expression[j] == bracketsClose)
                {
                    text = expression.mid(i + 1, j - i - 1);
                    conjunctionOrDisjunctionPriority(text);
                    table->insert(table->lastKey(), bracketsOpen + table->last() + bracketsClose);

                    if(i > 0 && expression[i - 1] == denial)
                    {
                        result= text;
                        text = "*" + QString::number(table->size()) + "*";
                        table->insert(table->size(), denial + result);
                        --i;
                    }
                    expression.replace(i, j - i + 1, text);
                    i = -1;
                    break;
                }
            }
        }
    }
}
/*Метод для опрацювання дій, які мають кон'юнкцію або диз'юнкцію*/
void Calculus::conjunctionOrDisjunctionPriority(QString &text)
{
    QStringList list = text.split(*regularExp);

    if(list.count() == 2)
    {
        int index = isDuplicate(text);
        if(index == -1)
        {
            index = table->size();
            table->insert(index, text);
        }
        text = "*" + QString::number(index) + "*";
    }
    else
    {
        QMap<int, QString> *orderActions = new QMap<int, QString>();

        for(int i = 0; i < list.count(); ++i)
        {
            orderActions->insert(i, list.at(i));
        }
        actionsAbbreviation(orderActions, text, conjunction);
        actionsAbbreviation(orderActions, text, disjunction);

        orderActions->clear();
        delete orderActions;

    }
    list.clear();
}
/*Метод для перевірки чи вже було обчисленно вказану дію*/
int Calculus::isDuplicate(const QString &text)
{
    QString swapText;
    int result = -1;
    QStringList list;
    for(QMap<int, QString>::iterator j = table->begin(); j != table->end(); ++j)
    {
        list = j.value().split(*regularExp);
        if(list.count() > 1)
        {
            if(list.at(0).at(0) == bracketsOpen)
            {
                swapText = bracketsOpen + deleteBrackets(list.at(1), bracketsClose) + j.value().at(j.value().indexOf(*regularExp)) + deleteBrackets(list.at(0), bracketsOpen) + bracketsClose;
            }
            else
            {
                swapText = list.at(1) + j.value().at(j.value().indexOf(*regularExp)) + list.at(0);
            }

            if(j.value() == text || j.value() == swapText)
            {
                result = j.key();
            }
        }
        else if(j.value() == text)
        {
            result = j.key();
        }
        list.clear();
    }
    return result;
}
/*Метод для заповнення словника, який відповідає за шапку таблиці*/
void Calculus::createHeaderTable()
{
    QString formula;
    QString action;
    QMap<QString, QString>::iterator it;
    QStringList list;
    QString bufferExpression;

    for(QMap<int, QString>::iterator i = table->begin(); i != table->end(); ++i)
    {
        formula = "";
        bufferExpression = i.value();
        bufferExpression.removeIf([this](const QString &value){ return value == bracketsOpen || value == bracketsClose; });
        list = bufferExpression.split(*regularExp);
        if(list.count() > 1)
        {
            action = bufferExpression.at(bufferExpression.indexOf(*regularExp));
            formula = getArgumentValue(list.at(0)) + action + getArgumentValue(list.at(1));
            if(i.value().at(0) == bracketsOpen)
            {
                formula = bracketsOpen + formula + bracketsClose;
            }
        }
        else
        {
            formula = getArgumentValue(i.value());
        }
        headerTable->insert(headerTable->size(), formula);
    }
}
/*Метод для окремих дій виразу*/
void Calculus::cleansing(QMap<QString, bool> &leterValue)
{
    bool result;
    QString text;
    QChar action;
    QStringList list;
    QString exeption_1, exeption_2;
    QString regStr;
    regStr.append(denial);
    regStr.append(bracketsOpen);
    regStr.append(bracketsClose);
    QRegularExpression *reg = new QRegularExpression("[*" + regStr + regularExpStr + "]");

    for(QMap<int, QString>::iterator i = table->begin(); i != table->end(); ++i)
    {
        text = i.value();
        list = text.split(*regularExp);
        exeption_1 = list.at(0);
        exeption_1.remove(*reg);
        if(list.count() > 1)
        {
            action = text.at(text.indexOf(*regularExp));
            exeption_2 = list.at(1);
            exeption_2.remove(*reg);

            if(action == conjunction)
            {
                result = getValue(exeption_1, leterValue) && getValue(exeption_2, leterValue);
            }
            else if(action == disjunction)
            {
                result = getValue(exeption_1, leterValue) || getValue(exeption_2, leterValue);
            }
        }
        else
        {
            result = !getValue(exeption_1, leterValue);
        }
        resultRow->insert(resultRow->size(), result);
    }
    delete reg;
}
/*Метод для повернення булевого значення*/
bool Calculus::getValue(QString element, QMap<QString, bool> &leterValue)
{
    if(element.length() == 1 && element.at(0).isLetter())
    {
        return leterValue.find(element.at(0)).value();
    }
    else
    {
        return resultRow->find(element.toInt()).value();
    }
}
/*Метод для обчислення виразу з для вказаної дії*/
void Calculus::actionsAbbreviation(QMap<int, QString> *orderActions, QString &text, const QChar &action)
{
    int index;
    QStringList actions;
    for(int i = 0; i < text.length(); ++i)
    {
        if(text.at(i) == conjunction || text.at(i) == disjunction)
        {
            actions.append(text.at(i));
        }
    }

    for(int i = 0; i < actions.count(); ++i)
    {
        if(actions.at(i) == action)
        {
            QString left = orderActions->value(i);
            QString right = orderActions->value(i + 1);
            QString result = left + action + right;
            index = isDuplicate(result);
            if(index == -1)
            {
                index = table->size();
                table->insert(index, result);
            }
            text.replace(result, "*"+QString::number(index)+"*");
            actions.remove(i);
            orderActions->insert(i, "*"+QString::number(index)+"*");

            for(int j = i + 1; j < orderActions->count() - 1; ++j)
            {
                orderActions->insert(j, orderActions->value(j + 1));
            }
            orderActions->remove(orderActions->count() - 1);

            i = -1;
        }
    }
}
/*Метод для отримання значення аргументу*/
QString Calculus::getArgumentValue(const QString &argument)
{
    bool denialCheck = argument.at(0) == denial;
    QString regStr = '*' + denial;
    QString buffer = argument;
    QString result;
    QRegularExpression *reg = new QRegularExpression("[" + regStr + regularExpStr + "]");
    buffer.remove(*reg);

    result = buffer.at(0).isLetter() ? buffer : headerTable->find(buffer.toInt()).value();
    if(denialCheck)
    {
        result = denial + result;
    }
    delete reg;
    return result;
}
/*Метод для видалення дужок виразу*/
QString Calculus::deleteBrackets(const QString &text, const QChar &brackets)
{
    QString buffer = text;
    return buffer.removeIf([brackets](const QChar &value) { return value == brackets; });
}
