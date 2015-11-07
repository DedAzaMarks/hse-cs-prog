// Найдите ошибку в этой программе!
// (такое пишут некоторые второкурсники сейчас, которые плохо учились в прошлом году :)

#include <iostream>

int main() {
    char * s;
    std::cin >> s;
    std::cout << s << "\n";
}

// Ответы из зала: 
// - Нельзя считывать и печатать указатель!
// (Конечно, можно: это же указатель на char, то есть, массив символов.)
// - Не освобождается память с помощью delete!
// (Конечно, delete в этой программе отстутствует, но главная проблема не в этом!)

// Ошибка в том, что здесь вообще не выделяется память!
// Вот мы объявили переменную s типа char *:
char * s;
// Какое значение в ней лежит?
// Это локальная переменная. Она никак не инициализируется, если мы явно об этом не попросим.
// Под хранение s выделена память на стеке (8 байт на моём компьютере).
// Но какое значение лежит в этих ячейках памяти - заранее неизвестно.
// То есть, эта s указывает куда-то в непонятное место, и к этой памяти нам наверняка запрещено обращаться!
// Мы пытаемся прочитать строку в эту непонятную область памяти, и это приводит к краху.

// Как решить проблему?
// Первый способ: выделяем динамическую память:
int main() {
    char * s = new char[100]; // рассчитываем, что 99 символов нам хватит (+ещё один нужен для хранения символа \0)
    std::cin >> s;
    std::cout << s << "\n";
    delete [] s;
}

// Второй способ: используем обычный статический массив, память под который выделяется на стеке:
int main() {
    char s[100];
    std::cin >> s;
    std::cout << s << "\n";
} // вручную ничего удалять не нужно: память, занимаемая s, сама освободится, когда мы дойдём до этой фигурной скобки


// Можно ли задавать статические массивы неизвестной заранее (на этапе компиляции) длины?
// Примерно так:
int main() {
    size_t N;
    std::cin >> N;
    int array[N];
}
// Раньше так было делать нельзя, и приходилось использовать именно динамический массив.
// Начиная с C++14 - можно.

// Так зачем же тогда нужны динамические массивы? Нельзя ли всегда обойтись статическими?
// Ответ: статические массивы, как и любые переменные, созданные на стеке, живут только до конца ближайшего блока.
// Если требуется, чтобы этот массив "пережил" конец блока, то нужно создавать его в динамической памяти.
// (Или, что лучше, использовать умную "обёртку" над этой динамической памятью - std::vector.)

// Например, пусть требуется вынести считывание данных в отдельную функцию.
// Найдите ошибку в этой программе!
int * read_array() { // Мы не знаем, сколько будет всего элементов в массиве. Поэтому возвращать из функции будем указатель на начало.
    size_t N;
    std::cin >> N;
    int array[N];
    for (size_t i = 0; i != N; ++i)
        std::cin >> array[i];
    return array; // Массив без проблем преобразуется к типу "указатель на int"
}

int main() {
    int * data = read_array();
    // что-то делаем с data
}

// Грубейшая ошибка в такой программе находится здесь:
int * read_array() {
    // ...
    int array[N];
    // ...
    return array; // возвращаем указатель на локально созданный статический массив
} // но здесь массив array "умирает", и указатель становится невалидным!

// Указатель, который возвращает такая функция, больше нельзя использовать.
// Память, на которую он указывал, уже не принадлежит массиву array. Там может быть размещено что-то другое.
// Попытка обратиться к ней приводит к неопределенному поведению.

// Массив array должен "пережить" выход из функции.
// Сделаем так:
int * read_array() {
    size_t N;
    std::cin >> N;
    int * array = new int[N]; // выделяем память под массив динамически
    for (size_t i = 0; i != N; ++i)
        std::cin >> array[i];
    return array; // возвращается копия значения указателя array.
} // сама переменная array тут умирает. Но это всего лишь указатель.
// Память, на которую он указывал, остаётся доступной.

int main() {
    int * data = read_array(); // получаем копию значения указателя array из функции
    // data ссылается на корректно выделенный участок динамической памяти, с ней можно работать.
    // только остаётся проблема - мы не знаем длину массива (она осталась в функции N).
    delete [] data; // мы вынуждены вручную освобождать память, когда она не будет нужна.
}

// Проще всего, конечно, сделать так:
std::vector<int> read_array() {
    size_t N;
    std::cin >> N;
    std::vector<int> array(N); // вектор сам внутри заботится о выделении и освобождении памяти
    for (size_t i = 0; i != N; ++i)
        std::cin >> array[i];
    return array; // возвращается копия объекта array
    // (на самом деле тут срабатывает move-семантика, но это тема отдельного рассказа)
}

int main() {
    const auto& data = read_array(); // можно написать const std::vector<int>&, на так короче
    // размер вектора нам известен - это data.size();
} // вручную удалять ничего не нужно - деструктор вектора очистит память сам, когда дойдёт до этой скобки



// Вопрос: что напечатает такая программа, и почему?

int main() {
     const char s1[] = "abcdef";
     const char * s2 = "abcdef";
     const char s3[10];
     std::cin >> s3;
     std::cout << sizeof(s1) << "\n";
     std::cout << sizeof(s2) << "\n";
     std::cout << sizeof(s3) << "\n";
}

// Ответ: 7, 8 и 10.
// s1 - это статический массив.
// Он инициализируется строкой из 6 символов, но в конце приписывается еще и символ с кодом 0.
// Итого получается 7 символов. Каждый символ занимает 1 байт. Получается, что sizeof(s1) == 7.

// С другой стороны, s2 - это указатель.
// Сейчас он ссылается на глобальную область памяти, в которой лежит строковая константа "abcdef".
// Вообще не важно, куда он ссылается. Его размер на стеке - 8 байт на моём компьютере (у меня 64-битная архитектура).

// s3 - тоже статический массив из 10 символов. Не важно, что в него ввели. Его sizeof равен 10.

// Мораль: не пытайтесь узнать длину строки с помощью sizeof :)


// Разберёмся, кстати, с этими C-строками, завершающимися символом с кодом 0.
// Как найти длину такой строки?
// Для этого в C была стандартная библиотечная функция strlen. Сейчас мы попробуем её написать самостоятельно.

size_t strlen(const char * s) { // я пишу const char, потому что саму строку изменять не буду
    size_t i = 0;
    while (s[i] != '\0') // перебираем символы, пока не дойдём до символа с кодом 0, и подсчитываем шаги
        ++i;
    return i;
}

int main() {
    char s[100]; // заведем для разнообразия статический массив
    std::cin >> s;
    std::cout << strlen(s) << "\n"; // на ввод "hello" печатает 5, как и ожидалось
}

// Под Visual Studio такой код может не заработать, так как определение нашей strlen может конфликтовать
// со стандартной strlen, которая подтягивается через заголовочный файл iostream.
// Если у вас происходит такая ошибка, то просто переименуйте эту функцию (например, в my_strlen)
// или оберните её в своё пространство имён:

namespace my_str_study {
    size_t strlen(const char * s) {
        // ...
    }
}

int main() {
    // ...
    std::cout << my_str_study::strlen(s) << "\n";
}


// Пусть теперь нам надо скопировать содержимое одной строки в другое место.
// Есть ли ошибка в таком коде?
int main() {
    char s1[] = "hello";
    char * s2 = new char[100];
    s2 = s1; // копируем ли мы здесь строку?
    std::cout << s2 << "\n"; // печатаем "hello" - может показаться, что всё хорошо
    delete [] s2; // тут внезапно большая проблема!
}

// В этом примере мы не скопировали саму строку "hello" в область памяти, на которую указывал s2
// Вместо этого мы заставляем указатель s2 указывать на часть стека, в котором хранится строка s1
// Проблема №1: мы "потеряли" указатель на выделенную динамическую память, и не сможем её корректно удалить
// Но это - полбеды. Проблема №2: delete [] s2 попытается удалить память на стеке, в которой хранится s1.

// Так как же скопировать строку? Это надо делать "глубоко", байт за байтом.
// В языке С для этого была стандартная функция strcpy.
// Напишем её.

// Первая версия (так обычно и пишут студенты):
void strcpy(char * to, const char * from) { // по названию аргументов понятно, что они значат: копируем из from в to
    // считаем, что память, на которую указывает to, уже корректно выделена в достаточном объёме
    size_t len = strlen(from);
    for (size_t i = 0; i != len; ++i)
        to[i] = from[i];
    to[len] = '\0';
}
// (на самом деле настоящая функция strlen из библиотеки cstring возвращает ещё указатель to,
// но мы для простоты будем считать, что она ничего не возвращает)

// Можно было бы избавиться от первой и последней строчки, написав так:
void strcpy(char * to, const char * from) {
    size_t i = 0;
    do {
        to[i] = from[i];
    } while (from[i] != '\0');
}

// А можно написать и вот такой классический вариант:
void strcpy(char * to, const char * from) {
    while (*to++ = *from++);
}

// Что здесь происходит?
// В ячейку, на которую указывал to, копируется символ из ячейки, на которую указывал from.
// Оба указателя в процессе вычисления этого выражения сдвигаются к следующим ячейкам.
// Результат присваивания - ссылка на тот объект, которому присваивали. То есть, это присвоенный символ.
// Этот результат превращается в логическое выражение по правилу: \0 - false, всё остальное - true.
// В результате цикл остановится после того, как будет присвоено значение \0.

