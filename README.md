# Практична робота "Розв'язання логічних виразів"

## Умова завдання

Побудувати програму для розв'язання простих логічних виразів з кон'юнкцією, диз'юнкцією та запереченням.

При запуску програма виглядає так:

![](https://github.com/BogdanPhoenix/DiscreteMath/raw/master/Image_Result/ProgramStart.png "Старт програми")

Після запуску програми, користувач може ввести вираз у відповідне поле та натиснути обчислити. Для прикладу можна обчислити наступні вирази:
1. A & (!C & B) V (D V !(B V A) & (C V D)) & C V !C <br>
![](https://github.com/BogdanPhoenix/DiscreteMath/raw/master/Image_Result/Example_1.png "Виконання прикладу №1")
2. A & !C & (D V !B V A) &C V !C <br>
![](https://github.com/BogdanPhoenix/DiscreteMath/raw/master/Image_Result/Example_2.png "Виконання прикладу №2")
3. A V !C & (B V !B V !A) V !C <br>
![](https://github.com/BogdanPhoenix/DiscreteMath/raw/master/Image_Result/Example_3.png "Виконання прикладу №3")
4. !A & !C & (!D V B & !C) V B & C <br>
![](https://github.com/BogdanPhoenix/DiscreteMath/raw/master/Image_Result/Example_4.png "Виконання прикладу №4")

Прототипи методів оброботки введених виразів знаходиться у файлі [calculus.h](/calculus.h), а опис методів - у [calculus.cpp](/calculus.cpp).

Початок обчислення, після натиску на кнопку **"Обчислити"** виконується у методі [priorityActions](/calculus.cpp). Сперш за все відбувається видалення пробілів між символами, для розбиття виразу на частини. Також перед початком обчислення виконується очищення матриці істинності, змінни _valuesVariables_ - список варіантів для кожної змінної, та змінни _valuesExpression_ - зберігає всі результати обчислень. Далі формується таблиця істинності.

За допомогою метода [calculateNumberVariables](/calculus.cpp) виконується виявлення змінних:

```c++
void Calculus::calculateNumberVariables(){
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
```

в результаті будуть створенні перші стовпчики таблиці.

Далі після генерування стартових значень відбувається пошук операцій або змінних із запереченням. Для цього використовується метод [denialPriority](/calculus.cpp), який в параметрі приймає посилання на вираз для опрацювання. Відбувається розбиття всього виразу на частини, розділювачами служать дії, окрім заперечення. Далі отриманні окремі частини виразу перевіряються на те, чи перший їх символ є символом заперечення _"!"_ та наступним після нього не йде символ відкривання дужок. Опрацювання заперечення виразу в дужках опрацьовується пізніше. Далі виконується перевірка на існування вже такої змінної в змінній _table_ (змінна для зберегання номеру дії та її значення, яке є зрозумілим для комп'ютера, деякі фрагменти замінені на цифри).
Після утворення індексу, у виразі дана частинка замінюється на індекс, який розташований між зірочками, даний спосіб виділення дозволяє обчислювати вирази, які мають більше десяти дій у своємо розв'язанні.

```c++
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
```

Перевірка на дублікат виразу відбувається в методі [isDuplicate](/calculus.cpp). Даний метод отримує в свої параметри вираз, який потрібно перевірити на наявність в таблиці.
Після отримання виразу виконується його розбиття на менші частинки без дій. Якщо таких частинок лише одна, то перевірка наявності в таблиці виконується відразу, минуючи утворення оберненого виразу.
Якщо після розбиття виразу на частинок їх більше ніж одна, то виконується утворення оберненого виразу, щоб виконати його пошук в таблиці даних. 

```c++
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
```

Наступним кроком є розбиття виразу на дії, які знаходяться в дужках. Цю роботу виконує метод [bracketsPriority](/calculus.cpp):

```c++
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
```

Спершу в методі [bracketsPriority](/calculus.cpp) здійснюється пошук дужки, що відкривається. Далі після виявлення відповідної дужки відбувається пошук дужки, що закриває вираз. Якщо на цьому шляху виявиться іще одна дужка, що відкривається, то номер попередньої дужки, що відкривається, замінюється на новознайдену. Після виявлення дужки, що закривається відбувається вирізання цієї частини з основного виразу. Далі ця частина передається в метод [conjunctionOrDisjunctionPriority](/calculus.cpp), де виконуються обчислення кон'юнкції та диз'юнкції. Після обислення всього виразу в результаті отримується лише індекс на шматок виразу. Цей шматок поміщається в таблицю, обернутий в дужки. Якщо ж перел дужками виявлено запережечення, то в таблицю _table_ додається іще одна дія, яка буде заперечувати вирізаний шматок. Після всіх маніпуляцій, індекс, в певному форматі (_\*індекс\*_) замінює вирізаний шматок.

В методі [conjunctionOrDisjunctionPriority](/calculus.cpp) наведено спосіб розбиття дій на кон'юнкцію та диз'юнкцію. Якщо вираз має лише одну дію (можна розкласти на дві частини), то виконується спершу перевірка на наявність такого виразу в попередніх діях, якщо не було виявлено схожої або оберненої дії, то виконується приствоєння індексу цій дії та створений індекс з дією додаються в таблицю для майбутніх перевірок.

```c++
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
```

Якщо у виразі виявлено більше однієї дії, то виконується покрокове розбиття виразу, спершу за кон'юнкцією, а далі за диз'юнкцією. Дане розбиття наведене у методі [actionsAbbreviation](/calculus.cpp).

```c++
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
```

Після розбиття виразів у дужках початковий вираз позбувається їх. Це дає змогу виконати кінцеве розбиття на дії за допомогою метода [conjunctionOrDisjunctionPriority](/calculus.cpp).

Наступним в черзі іде створення заголовків кожного з стовпчиків, які відповідають за конкретну дію. Опис створення заголовків стовпчиків розписаний в методі [createHeaderTable](/calculus.cpp):

```c++
void Calculus::createHeaderTable(){
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
```

Принцип дії цього метода простий, береться попередньо розбитті дії і з них формуються заголовки.

І в кінці метода [priorityActions](/calculus.cpp) викликається інший метод - [cleansing](/calculus.cpp), який виконує обчислення матриці:

```c++
void Calculus::cleansing(QMap<QChar, bool> &leterValue){
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
```

Даний метод бере і виконує ті дії, які були розбитті до цього на частини. За допомогою метода [getValue](/calculus.cpp), в залежності від типу елемента (цілочисельний або символьний), вибирається відповідний результат. Якщо символ є цілочисельного типу, то повертається результат обчислення тієї дії, якої вказане число, а інакте повертається результат тієї змінної, якій відповідає даний символ.

```c++
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
```