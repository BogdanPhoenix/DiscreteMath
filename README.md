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
    ...

    for(int i = 0; i < expression.count(); ++i){
        if(expression[i].isLetter() && expression[i].toLower() != disjunction){
            buffer->insert(expression[i]);
        }
    }
    bufferSort = buffer->values();
    std::sort(bufferSort.begin(), bufferSort.end(), [](const QChar a, const QChar b){ return (a < b); });

    ...
}
```

та генерування їх значень:
```c++
void Calculus::calculateNumberVariables(){
    ...

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

    ...
}
```

в результаті будуть перші стовпчики таблиці.

Далі після генерування ствртових значень відбувається пошук операцій або змінних із запереченням. Для цього використовується метод [denialPriority](/calculus.cpp), який в параметрі приймає посилання на вираз для опрацювання. Відбувається покрокова перевірка кожного елемента на заперечення. Якщо заперечення виявлено, і воно знаходиться не перед відкриваючою дужкою, то виконується заміна змінної із запереченням на відповідний номер дії. Але спершу виконується перевірка на існування вже такої змінної в змінній _table_ (змінна для зберегання номеру дії та її значення, яке є зрозумілим для комп'ютера, деякі фрагменти замінені на цифри).

```c++
void Calculus::denialPriority(QString &text){
    int index;
    QString findDenial;
    for(int i = 0; i < text.length() - 1; ++i){
        if(text[i] == denial && text[i + 1] != bracketsOpen){
            findDenial = text.mid(i, 2);
            index = isDuplicate(findDenial);
            if(index < 0){
                table->insert(table->size(), findDenial);
                index = table->size() - 1;
            }
            text = text.replace(findDenial, QString::number(index));
            --i;
        }
    }
}
```

Перевірка на дублікат виразу відбувається в методі [isDuplicate](/calculus.cpp)

```c++
int Calculus::isDuplicate(QString &text){
    for(QMap<int, QString>::iterator j = table->begin(); j != table->end(); ++j){
        if(j.value() == text){
            return j.key();
        }
    }
    return -1;
}
```
Наступним кроком є розбиття виразу на дії, які знаходяться в дужках. Цю роботу виконує метод [bracketsPriority](/calculus.cpp):

```c++
void Calculus::bracketsPriority(){
    QString result, text;
    QMap<int, QString>::iterator it;
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
                        it = table->find(text.toInt());
                        it.value() = bracketsOpen + it.value() + bracketsClose;
                    }
                    if(i > 0 && expression[i - 1] == denial){
                        result = denial + QString::number(table->size() - 1);
                        table->insert(table->size(),result);
                        --i;
                    }
                    expression.replace(i, j - i + 1, QString::number(table->size() - 1));
                    i = -1;
                    break;
                }
            }
        }
    }
}
```

Спершу в методі [bracketsPriority](/calculus.cpp) здійснюється пошук дужки, що відкривається. Далі після виявлення відповідної дужки відбувається пошук дужки, що закриває вираз. Якщо на цьому шляху виявиться іще одна дужка, що відкривається, то номер попередньої дужки, що відкривається, замінюється на новознайдену. Після виявлення дужки, що закривається відбувається розбиття виразу, що знаходиться між дужками, на дії. Спершу здійснюється пошук дій з кон'юнкцією, а після - з диз'юнкцією. Ці перевірки здійснює метод [conjunctionOrDisjunctionPriority](/calculus.cpp), що приймає в параметри посилання на вираз та на логічний символ.

```c++
void Calculus::conjunctionOrDisjunctionPriority(QString &text, QChar element){
    QString sub;
    int index;
    for(int i = 1; i < text.length() - 1; ++i){
        if(text[i].toLower() == element){
            sub = text.mid(i - 1, 3);
            index = isDuplicate(sub);
            if(index < 0){
                table->insert(table->size(), sub);
                index = table->size() - 1;
            }
            text.replace(sub, QString::number(index));
            i = 0;
        }
    }
}
```

Після розбиття виразу на дії виконується перевірка на кількість символів залишилося у виразі і це числове значення розташовується між дужками і додається в змінну _table_ для зберігання. Якщо перед дужкою стояло заперечення, то в таблицю додається заперечення номеру дії, щоб після її виконання виконати необхідне обчислення. І після всіх обчислень виразу, в початковому виразі відбувається заміна на номер дії та присвоююється початковий номер для пошуку нової дужки.

Після розбиття виразів у дужках початковий вираз позбувається їх. Це дає змогу виконати кінцеве розбиття на дії за допомогою метода [conjunctionOrDisjunctionPriority](/calculus.cpp).

Наступним в черзі іде створення заголовків кожного з стовпчиків, які відповідають за конкретну дію. Опис створення заголовків стовпчиків розписаний в методі [createHeaderTable](/calculus.cpp):

```c++
void Calculus::createHeaderTable(){
    QString text;
    QMap<int, QString>::iterator it;
    for(QMap<int, QString>::iterator i = table->begin(); i != table->end(); ++i){
        text = i.value();
        for(int j = 0; j < text.length(); ++j){
            if(text[j].isDigit()){
                it = table->find(text[j].digitValue());
                text = text.mid(0, j) + it.value() + text.mid(j + 1);
                --j;
            }
        }
        headerTable->insert(headerTable->size(), text);
    }
}
```

Принцип дії цього метода простий, береться попередньо розбитті дії і з них формуються заголовки.

І в кінці метода [priorityActions](/calculus.cpp) викликається інший метод - [cleansing](/calculus.cpp), який виконує обчислення матриці:

```c++
void Calculus::cleansing(QMap<QChar, bool> &leterValue){
    QString text;
    bool result;
    for(QMap<int, QString>::iterator i = table->begin(); i != table->end(); ++i){
        text = i.value();
        for(int j = 0; j < text.length(); ++j){
            if(text[j] == denial){
                result = !getValue(text[j + 1], leterValue);
                break;
            }
            else if(text[j] == conjunction){
                result = getValue(text[j-1], leterValue) && getValue(text[j + 1], leterValue);
                break;
            }
            else if(text[j].toLower() == disjunction){
                result = getValue(text[j-1], leterValue) || getValue(text[j + 1], leterValue);
                break;
            }
        }
        resultRow->insert(resultRow->size(), result);
    }
}
```

Даний метод бере і виконує ті дії, які були розбитті до цього на частини. За допомогою метода [getValue](/calculus.cpp), в залежності від типу елемента (цілочисельний або символьний), вибирається відповідний результат. Якщо символ є цілочисельного типу, то повертається результат обчислення тієї дії, якої вказане число, а інакте повертається результат тієї змінної, якій відповідає даний символ.

```c++
bool Calculus::getValue(QChar element, QMap<QChar, bool> &leterValue){
    if(element.isDigit()){
        return resultRow->find(element.digitValue()).value();
    }
    else{
        return leterValue.find(element).value();
    }
}
```