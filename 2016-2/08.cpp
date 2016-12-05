// Напишем класс "Дата", хранящий сведения о числе, месяце и годе, и умеющий выполнять арифметические операции с датами.

class Date {
  private:
    int day, month, year;

  public:
    Date(int d, int m, int y)
        : day(d)    // догадайтесь, почему удобно использовать такое выравнивание при инициализации полей?
        , month(m)
        , year(y)
    {
        // Тут хорошо бы сделать проверку на корректность, и в случае проблем сгенерировать исключение
    }

    int GetDay() const {
        return day;
    }

    int GetMonth() const {
        return month;
    }

    int GetYear() const {
        return year;
    }

    Date& operator ++ () { // префиксный ++
        // добавляем один день к дате...
        return *this;
    }

    Date& operator -- () { // префиксный --
        // вычитаем один день из даты...
        return *this;
    }

    int operator - (const Date& other) const { // оператор "минус" для количества дней между датами
        // поскольку мы реализуем этот оператор внутри класса,
        // то его левая часть этого оператора передается неявно - это текущий объект, на него указывает *this,
        // a other - это правая часть нашего оператора

        // вычисляем и возвращаем количество дней между other и *this...
    }
};

// Обратите внимение, некоторые функции-члены класса мы объявили константными (со словом const в конце).
// Дело в том, что компилятор разрешает применять к константам только "константные" функции, чтобы быть уверенным, что не изменят объект.
// В свою очередь, в каждой "константной" функции компилятор тоже делает такую проверку.

// Мораль: если функция-член класса по смыслу не меняет состояние объекта, объявите её константной.
// Можно сказать и по-другому: в константную функцию неявно передается не указатель Complex * const this, а указатель const Complex * const this.

// Использовать нашу дату можно так:
int main() {
    Date d2 {3, 10, 2016};
    ++d2;
    std::cout << (d1 - d2) << "\n";
}


// Вопрос: а для чего, собственно, мы делаем поля day, month и уear закрытыми (приватными)?
// Во-первых, в нашей реализации класса мы так защищаем их от несанкционированного изменения:
// внешний код их может только читать с помощью функций-getter'ов, но не может напрямую изменять.

// Во-вторых, это общий принцип: скрывать внутренние детали реализации, и открывать только общий интерфейс.
// Тогда детали реализации можно будет при необходимости изменить, не меняя код всех внешних программ, которые используют наш класс.

// Предположим, например, что мы решили поменять реализацию:
// вместо дня, месяца и года хотим хранить число дней, прошедших с начала некоторой эпохи.
// Спокойно меняем private-поля, переписываем открытые функции-члены.
// А все внешние программы этого даже не замечают, поскольку они private-поля и не могли использовать.

class Date {
private:
    int days_from_beginning;
public:
    Date(int d, int m, int y) {
        // некоторые действия по инициализации нашего нового поля
    }

    int GetDay() const; // эти функции становятся уже не такими тривиальными...
    int GetMonth() const;
    int GetYear() const;

    // Зато вот эти функции теперь пишутся легко:
    Date& operator ++ () {
        ++days_from_beginning;
        return *this;
    }

    Date& operator -- () {
        --days_from_beginning;;
        return *this;
    }

    int operator - (const Date& other) const {
        return days_from_beginning - other.days_from_beginning;
    }
};


// Вопрос из зала: а как перегружать постфиксный ++?
// Ответ: надо объявить его с фиктивным параметром типа int:
Date operator ++ (int) {
    Date temp(*this);
    ++days_from_beginning;
    return temp;
}

// Помните, что канонический постфиксный ++ (для чисел) возвращает копию старого (неизмененного) объекта?
// Поэтому и для дат имеет смысл реализовывать его именно так, как написано.

// Кстати, обратите внимание, что очень важно в этом случае вернуть Date, а не Date&.
// Если бы мы возвращали тут Date&, то получилась бы невалидная ссылка на временный объект temp,
// который при достижении закрывающей фигурной скобки умирает.

// Вопрос: в чём разница между this->days_from_beginning и просто days_from_beginning?
// Ответ: разницы нет, но второй способ лаконичнее и предпочтительнее.



// Теперь начнём писать класс для квадратных матриц фиксированного размера.
// Он будет шаблонным: параметры шаблона - это тип элементов матрицы и её размер.

#include <iostream>
#include <vector>

using std::vector;

template <typename T, int N>
class Matrix {
 private:
    vector<vector<T>> data;
    // тут могут быть и другие реализации: например, фиксированный массив T[N][N], или одномерный массив размера N^2

 public:
    Matrix(const vector<vector<T>>& d)
    : data(d) {
    }

    // Конструктор для скалярной матрицы
    Matrix(const T& value = 0) {
        data.resize(N);
        for (auto& row : data)
            row.resize(N);
        for (size_t i = 0; i != N; ++i)
            data[i][i] = value;
    }

    // Перегрузим оператор "круглые скобки" для получения элемента по индексам.
    // Это "неконстантная" версия:
    T& operator() (size_t i, size_t j) {
        return data[i][j];
    }

    // Это - "константная" версия.
    const T& operator() (size_t i, size_t j) const {
        return data[i][j];
    }

    // Левая часть этого оператора - это текущий объект (*this)
    // Правая часть - это параметр other.
    // Традиционный += возвращает ссылку на сам изменённый объект.
    Matrix& operator += (const Matrix& other) {
        for (size_t i = 0; i != N; ++i)
            for (size_t j = 0; j != N; ++j)
                data[i][j] += other.data[i][j];
                // Можно было бы написать (*this)(i, j) += other(i, j);
        return *this;
    }

    // Обычный + возвращает новую матрицу, а исходную левую часть не изменяет.
    // Поэтому этот оператор константный.
    Matrix operator + (const Matrix& other) const {
        auto C = *this;
        C += other;  // реализуем + через +=
        return C;
    }

    // Остальные операторы (-, -=, *, *=, ==, !=, унарный -) допишите сами.
};

// Оператор сложения можно было бы сделать и "внешним" по отношению к классу.
// Только тогда доступа к приватному полю data у нас уже бы не было.
template <typename T, int N>
Matrix<T, N> operator + (
    const Matrix<T, N>& A,
    const Matrix<T, N>& B
) {
    Matrix<T, N> C;
    for (size_t i = 0; i != N; ++i)
        for (size_t j = 0; j != N; ++j)
            C(i, j) = A(i, j) + B(i, j);
    return C;
}

template <typename T, int N>
void print(const Matrix<T, N>& m) {
    for (size_t i = 0; i != N; ++i) {
        for (size_t j = 0; j != N; ++j)
            std::cout << m(i, j) << "\t";  // какая версия operator() здесь вызовется?
        std::cout << "\n";
    }
}

int main() {
    const int N = 3;
    Matrix<int, N> A(7);
    
    Matrix<int, N> B(-1);
    B(1, 2) = 5;
    B(2, 0) = -4;

    print(A);
    std::cout << "==========\n";
    print(B);
    std::cout << "==========\n";
    print(A + B);
}

