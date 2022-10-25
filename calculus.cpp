#include "calculus.h"

/*Конструктор*/
Calculus::Calculus()
{
    table = new QMap<QString, QString>();
    headerTable = new QMap<int, QString>();
    valuesVariables = new QMap<QChar, QList<bool>>();
    valuesExpression = new QList<QMap<int, bool>>();
}
/*Деструктор*/
Calculus::~Calculus(){
    delete table;
    delete headerTable;
    delete valuesVariables;
    delete valuesExpression;
    if(!resultRow->isEmpty()){
        delete resultRow;
    }
}
/*Метод для отримання виразу*/
void Calculus::setExpression(QString text){
    this->expression = text;
}
/*Метод для обчислення вказаного виразу*/
void Calculus::priorityActions(){
    QMap<QChar, bool> *result;
    //Видалення всіх пробілів
    expression = expression.simplified();
    expression.replace(" ", "");

    table->clear();
    headerTable->clear();
    for(QMap<QChar, QList<bool>>::iterator i = valuesVariables->begin(); i != valuesVariables->end(); ++i){
        i.value().clear();
    }
    valuesVariables->clear();
    for(QList<QMap<int, bool>>::iterator i = valuesExpression->begin(); i != valuesExpression->end(); ++i){
        i->clear();
    }
    valuesExpression->clear();

    calculateNumberVariables();
    denialPriority(expression);
    bracketsPriority();
    conjunctionOrDisjunctionPriority(expression, conjunction);
    conjunctionOrDisjunctionPriority(expression, disjunction);
    createHeaderTable();

    for(int i = 0; i < qPow(2, numberVariables); ++i){
        result = new QMap<QChar, bool>();
        resultRow = new QMap<int, bool>();
        //Формування фрагменту значень елементів, які будуть задіюватися для обчислення
        for(QMap<QChar, QList<bool>>::iterator it = valuesVariables->begin(); it != valuesVariables->end(); ++it){
            result->insert(it.key(), it.value().at(i));
        }
        cleansing(*result);
        valuesExpression->push_back(*resultRow);
        delete result;
    }
}
/*Метод для формування таблиці з результатами*/
void Calculus::fillingTable(QTableWidget &tableWork){
    int index;
    QMap<QChar, QList<bool>>::iterator it_Variables = valuesVariables->begin();
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
        for(it_Variables = valuesVariables->begin(); it_Variables != valuesVariables->end(); ++it_Variables){
            tableWork.setItem(i, index++, new QTableWidgetItem(QString::number(it_Variables.value().at(i))));
        }
        for(QMap<int, bool>::iterator its = it_Expression->begin(); its != it_Expression->end(); ++its){
            tableWork.setItem(i, index++, new QTableWidgetItem(QString::number(its.value())));
        }
        ++it_Expression;
    }
}
/*Метод для визначення аргументів, створення таблиці з їх даними*/
void Calculus::calculateNumberVariables(){
    int num;
    bool push;
    QList<bool> *list;
    QList<QChar> bufferSort;
    QSet<QChar> *buffer = new QSet<QChar>();

    //Отримання унікальних аргументіів
    for(int i = 0; i < expression.length(); ++i){
        if(expression[i].isLetter() && expression[i] != disjunction){
            buffer->insert(expression[i]);
        }
    }
    bufferSort = buffer->values();
    std::sort(bufferSort.begin(), bufferSort.end(), [](const QChar a, const QChar b){ return (a < b); });

    numberVariables = bufferSort.count();
    num = numberVariables - 1;

    //Заповнення словника значеннями для кожного аргументу
    for(int i = 0; i < bufferSort.count(); ++i){
        push = false;
        list = new QList<bool>();
        for(int j = 0; j < numberVariables * numberVariables; ++j){
            if(j % (int)qPow(2, num) == 0){
                push = !push;
            }
            list->push_back(push);
        }
        valuesVariables->insert(bufferSort[i], *list);
        --num;
        delete list;
    }

    delete buffer;
}
/*Метод для пошуку елементів, які маю у своєму складі заперечення*/
void Calculus::denialPriority(QString &text){
    QString index, findDenial;
    for(int i = 0; i < text.length() - 1; ++i){
        if(text[i] == denial && text[i + 1] != bracketsOpen){
            findDenial = text.mid(i, 2);
            index = isDuplicate(findDenial);
            if(!index.length()){
                index = "*" + QString::number(table->size()) + "*";
                table->insert(index, findDenial);
            }
            text = text.replace(findDenial, index);
            --i;
        }
    }
}
/*Метод для визначення виразі, які розміщенні в дужках та подальше розкладання їх на дії*/
void Calculus::bracketsPriority(){
    QString result, text, index;
    QMap<QString, QString>::iterator it;
    for(int i = 0; i < expression.length() - 1; ++i){
        if(expression[i] == bracketsOpen){
            for(int j = i + 1; j < expression.length(); ++j){
                if(expression[j] == bracketsOpen){
                    i = j;
                }
                else if(expression[j] == bracketsClose){
                    text = expression.mid(i + 1, j - i - 1);
                    conjunctionOrDisjunctionPriority(text, conjunction);
                    conjunctionOrDisjunctionPriority(text, disjunction);
                    if(text.length() == 1 && text[0].isDigit()){
                        it = table->find("*" + text + "*");
                        it.value() = bracketsOpen + it.value() + bracketsClose;
                    }
                    index = "*" + QString::number(table->size()) + "*";
                    if(i > 0 && expression[i - 1] == denial){
                        result = denial + QString::number(table->size() - 1);
                        table->insert(index, result);
                        --i;
                    }
                    expression.replace(i, j - i + 1, index);
                    i = -1;
                    break;
                }
            }
        }
    }
}
/*Метод для опрацювання дій, які мають кон'юнкцію*/
void Calculus::conjunctionOrDisjunctionPriority(QString &text, QChar element){
    QString sub, index;
    for(int i = 1; i < text.length() - 1; ++i){
        if(text[i] == element){
            sub = text.mid(i - 1, 3);
            index = isDuplicate(sub);
            if(!index.length()){
                index = "*" + QString::number(table->size()) + "*";
                table->insert(index, sub);
            }
            text.replace(sub, index);
            i = 0;
        }
    }
}
/*Метод для перевірки чи вже було обчисленно вказану дію*/
QString Calculus::isDuplicate(QString &text){
    for(QMap<QString, QString>::iterator j = table->begin(); j != table->end(); ++j){
        if(j.value() == text){
            return j.key();
        }
    }
    return "";
}
/*Метод для заповнення словника, який відповідає за шапку таблиці*/
void Calculus::createHeaderTable(){
    QString text;
    QMap<int, QString>::iterator it;
    for(QMap<QString, QString>::iterator i = table->begin(); i != table->end(); ++i){
        text = i.value();
        foreach (auto split, text.split(QRegularExpression("&!V")))
        {
            qDebug() << split;
        }
        /*for(int j = 0; j < text.length(); ++j){
            if(text[j].isDigit()){
                it = table->find(text[j].digitValue());
                text = text.mid(0, j) + it.value() + text.mid(j + 1);
                --j;
            }
        }*/
        headerTable->insert(headerTable->size(), text);
    }
}
/*Метод для окремих дій виразу*/
void Calculus::cleansing(QMap<QChar, bool> &leterValue){
    QString text;
    bool result;

    for(QMap<QString, QString>::iterator i = table->begin(); i != table->end(); ++i){
        text = i.value();
        for(int j = 0; j < text.length(); ++j){
            if(text[j] == denial){
                result = !getValue(chechSymbol(text, j, Direction::Rigth), leterValue);
                break;
            }
            else if(text[j] == conjunction){
                result = getValue(chechSymbol(text, j, Direction::Left), leterValue) && getValue(chechSymbol(text, j, Direction::Rigth), leterValue);
                break;
            }
            else if(text[j] == disjunction){
                result = getValue(chechSymbol(text, j, Direction::Left), leterValue) || getValue(chechSymbol(text, j, Direction::Rigth), leterValue);
                break;
            }
        }
        resultRow->insert(resultRow->size(), result);
    }
}
/*Метод для повернення булевого значення*/
bool Calculus::getValue(QString element, QMap<QChar, bool> &leterValue){
    if(element.length() == 1 && element.at(0).isLetter())
    {
        return leterValue.find(element.at(0)).value();
    }
    else
    {
        return resultRow->find(element.toInt()).value();
    }
}

QString Calculus::chechSymbol(QString &text, int indexStart, Direction direction)
{
    QString result;
    int index = direction == Direction::Left ? -1 : 1;
    if(text[indexStart + index] != '*')
    {
        result = text[indexStart + index];
    }
    else
    {
        result = findNumber(text, indexStart, direction);
    }
    return result;
}

QString Calculus::findNumber(QString &text, int indexStart, Direction direction)
{
    QString result;

    if(direction == Direction::Left)
    {
        for(int i = indexStart - 2; i > 0; --i)
        {
            if(text[i] == '*')
            {
                break;
            }
            result += text[i];
        }
        std::reverse(result.begin(), result.end());
    }
    else if(direction == Direction::Rigth)
    {
        for(int i = indexStart + 2; i < text.length(); ++i)
        {
            if(text[i] == '*')
            {
                break;
            }
            result += text[i];
        }
    }

    return result;
}

















