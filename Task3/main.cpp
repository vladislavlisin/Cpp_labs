#include <iostream>
#include <string>

using namespace std;

// модифицированный класс матриц из первого тура - ключ для хэш-таблицы (часть реализации)
class [[maybe_unused]] Matrix
{
private:

    int **m_matrix; // квадратная матрица
    int m_size; // её размер

public:
    friend istream& operator>> (std::istream &in, Matrix &matrix);
    friend ostream& operator<< (std::ostream &out, const Matrix &matrix);
    friend bool operator== (const Matrix &matrix1, const Matrix &matrix2);
    friend bool operator!= (const Matrix &matrix1, const Matrix &matrix2);

    Matrix(const Matrix& matrix) // конструктор копирования
    {
        m_size = matrix.m_size;
        m_matrix = new int* [m_size];
        for(int i = 0; i < m_size; i++)
        {
            m_matrix[i] = new int[m_size];
            for(int j = 0; j < m_size; j++)
            {
                m_matrix[i][j] = matrix.m_matrix[i][j];
            }
        }
    }

    static void Swap(Matrix& matrix1, Matrix& matrix2)
    {
        swap(matrix1.m_size, matrix2.m_size);
        swap(matrix1.m_matrix, matrix2.m_matrix);
    }

    Matrix& operator= (const Matrix& matrix) {
        Matrix temp{matrix};
        Swap(*this, temp);
        return *this;
    }

    Matrix() // Конструктор по умолчанию для инициализации матрицы размерности 0
    {
        m_matrix = nullptr;
        m_size = 0;
    }

    [[maybe_unused]] [[nodiscard]] int get_key() const
    {
        int sum = 0;
        for(int i = 0; i < m_size; i++) {
            for (int j = 0; j < m_size; j++) {
                sum += m_matrix[i][j];
            }
            sum *= 2;
        }
        return sum;
    }

    void resize(int size) // меняем размер матрицы и выделяем под неё память
    {
        m_size = size;
        m_matrix = new int *[m_size];
        for (int j = 0; j < m_size; j++)
        {
            m_matrix[j] = new int[m_size]();
            m_matrix[j][j] = 1;
        }
    }

    ~Matrix()
    {
        if (m_size != 0)
        {
            for (int i = 0; i < m_size; ++i)
                delete[] m_matrix[i];
            delete[] m_matrix;
        }
    }
};

bool operator!= (const Matrix &matrix1, const Matrix &matrix2) // неравенство двух матриц
{
    return !(matrix1 == matrix2);
}

bool operator== (const Matrix &matrix1, const Matrix &matrix2) // равенство двух матриц
{
    if(matrix1.m_size != matrix2.m_size) return false; // т.к если размерности матриц не совпадают, значит они различны

    for(int i = 0; i < matrix1.m_size; i++)
    {
        for(int j = 0; j < matrix1.m_size; j++)
        {
            if (matrix1.m_matrix[i][j] != matrix2.m_matrix[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

std::ostream& operator<< (std::ostream &out, const Matrix &matrix)
{
    for(int i = 0; i < matrix.m_size; i++)
    {
        for(int j = 0; j < matrix.m_size; j++)
        {
            out << matrix.m_matrix[i][j] << " ";
        }
        out << "\n";
    }
    return out;
}

std::istream& operator>> (std::istream &in, Matrix &matrix)
{
    int size;
    in >> size;
    matrix.resize(size); // делаем единичную матрицу указ размера
    for(int i = 0; i < matrix.m_size; i++)
    {
        for(int j = 0; j < matrix.m_size; j++)
        {
            in >> matrix.m_matrix[i][j];
        }
    }
    return in;
}

// Массив в котором будут храниться указатели на связные списки для цепочек - Vector
template <typename L>
class Vector
{
private:
    L* vector;
    int size = 0;

    void init(const Vector<L> &that)
    {
        size = that.size; // изменяем размер массива
        vector = new L[size]; // выделяем память
        for(int i = 0; i < size; ++i)
            vector[i] = that.vector[i];
    }
public:

    Vector() : size(0), vector(nullptr) {}

    Vector(int _size) : vector(nullptr) { resize(_size); }

    Vector(const Vector<L> &that)
    {
        init(that);
    }

    Vector<L> &operator= (const Vector<L> &that)
    {
        if(this != &that) // проверка на присвоение самого себя
            init(that);
        return *this;
    }

    void resize(int new_size, L value)
    {
        auto new_vector = new L[new_size];
        for(int i = 0; i < new_size; ++i)
            new_vector[i] = value;
        delete[] vector;
        vector = new_vector;
        size = new_size;
    }

    void resize(int new_size)
    {
        auto new_vector = new L[new_size];
        delete[] vector;
        vector = new_vector;
        size = new_size;
    }

    L &operator[] (int index) { return vector[index]; }
    typedef L* iterator;
    [[maybe_unused]] int get_size() { return size; }
    [[nodiscard]] iterator begin() const { return vector; } // возвращает указатель на начало массива
    [[nodiscard]] iterator end() const { return vector + size; } // возвращает указатель на конец массива
    ~Vector() { delete[] vector; }
};

namespace std
{
    template<>
    class hash<Matrix>
            {
    public:
        int operator()(Matrix const& matrix) const noexcept
        {
            return matrix.get_key();
        }
    };
}

// Сама хэш-таблица
// Добавление - если элемент с таким ключом уже есть в хеш-таблице, его значение должно быть обновлено на value
template <typename K, typename V>
class HashMap
{
private:

    // реализуем собственные стандратные классы list и vector
    // Односвязный список для обхода коллизий методом цепочек - List
    class List
    {
    public:
        K key;
        V value;
        List *next;

        List(K& key, V& value) : key(key), value(value), next(nullptr) {}
    };

    Vector<List*> table; // массив из указателей на односвязные списки - хэштаблица
    int size = 0; // размер таблицы
    int elements_number = 0; // количество всех элементов в таблице
    double sparsity_percentage = 0.8; // процент разряженности таблицы (для данной 70 процентов)

public:
    List* operator[] (const int index) { return table[index]; }

    int hash_func(K &key)
    {
        hash<K> hash_value;
        return hash_value(key) % size;
    }

    class iterator;

    // пусть функция возвращает итератор на элемент данного ключа
    iterator find(K& key)
    {
        int index = hash_func(key);
        // если ключа нет вернуть нулевой итератор
        if(!table[index]) return end();

        // итератор на текущий элемент в списке на текущий элемент в массиве
        iterator element = iterator(table[index], table.begin() + index, table.end());
        auto here_vec_it = element.get_vec_it(); //

        // двигаемся по хэш-таблице - ищем элемент под данным ключём
        while(true)
        {
            if(element.get_vec_it() != here_vec_it) return end(); // двигаемся по конкретной ячейке массива
            if(element->key == key) return element; // нашли нужный ключ, возвращаем найденный итератор-элемент
            ++element;
        }
    }

    void add(K key, V value)
    {
        iterator element = find(key);
        // если указатель на текущий список не пуст - устанавливаем в нём новое значение, затирая при этом старое
        if(element != end())
            element->value = value; // стрелочка перегружена
        else if(element == end())
        {
            // иначе если пуст, создаём хэш значение и индекс для ключа, проверяем на рязреженность
            check_resize(); // проверка хэш-таблицы на разреженность, а уже только потом находим индекс
            int index = hash_func(key);
            auto ptr = new List(key, value);
            // создаём новый и инициализируем его данными значениями key and value
            // иначе если не нулевой создаём новый список,
            // список в ячейке index, то есть добавляем узелок в самое начало
            ptr->next = table[index];
            table[index] = ptr;
            elements_number++;
        }
    }

    void remove(K key)
    {
        iterator element = find(key);
        // если указатель на текущий список пуст - завершаем процесс (нечего удалять потому что)
        if(element == end()) return;
        // если существует предыдущий и текущий элемент в списке, то prev.next = it.next (prev -> it -> it.next)
        if(element.get_prev())
            element.get_prev()->next = element->next;
        // иначе если прерыдущего узелка нет, устанавливаем it.next следующий после it в самое начала ячейки
        else
            *(element.get_vec_it()) = element->next;
        elements_number--; // счётчик количества элементов
        delete element.get_it();
    }

    using vec_iter = typename Vector<List*>:: iterator;

    // Служебный класс Iterator для перемещения по хэш-таблице
    class iterator
    {
    private:
        List* it; // Указатель на текущий узелок списка
        List* prev; // указатель на предыдущий узелок списка
        vec_iter vec_it; // указатель на текущий элемент массива
        vec_iter vec_end; // указатель на конец массива

    public:
        iterator(List* it, vec_iter vec_it, vec_iter vec_end)
        {
            this->it = it;
            this->vec_it = vec_it;
            this->vec_end = vec_end;
            this->prev = nullptr;
        }

        iterator(const iterator& that): it(that.it), vec_it(that.vec_it), vec_end(that.vec_end), prev(that.prev) {}

        iterator &operator++()
        {
            // движение по списку
            if(it->next)
            {
                prev = it;
                it = it->next;
                return *this;
            }
            prev = nullptr;

            // движение по массиву, пока текущий указатель не станет указателем на конец массива
            vec_it++;
            while(vec_it != vec_end)
            {
                // если arr_it не пусто, то
                if(*vec_it)
                {
                    it = *vec_it;
                    return *this;
                }
                // иначе двигаемся дальше по массиву
                vec_it++;
            }
            it = nullptr;
            return *this;
        }

        vec_iter get_vec_it() { return vec_it; }

        bool operator==(const iterator& that) const
        {
            return ((it == that.it) && (vec_it == that.vec_it) && (vec_end == that.vec_end));
        }
        bool operator!= (const iterator& that) const { return !(*this == that); }

        List* operator-> () { return it; }
        List& operator* () { return *it; }
        List* get_prev () { return prev; }
        List* get_it () { return it; }
    };

    iterator begin() const {
        for(vec_iter it = table.begin(); it != table.end() ; it++)
        {
            if(*it) return iterator(*it , it , table.end());
        }
        return end();
    }

    iterator end() const { return iterator(nullptr, table.end(), table.end()); }

    [[maybe_unused]] [[nodiscard]] int get_el_number() const { return elements_number; }

    // стандартный конструктор
    explicit HashMap(int size)
    {
        this->size = size;
        table.resize(size, nullptr);
        elements_number = 0;
    }

    // конструктор копирования
    HashMap(HashMap& map)
    {
        this = map;
    }

    void print_hashtable() const
    {
        for(auto it = begin(); it != end(); ++it)
        {
            cout << it->key << " " << it->value << endl;
        }
    }

    // нужно всё хорошо скопировать, размер таблицы(количество списков), сумарное количество вершин у списков,
    // увеличить сам массив списков (или уменьшить), и все узелки скопировать в новую талицу
    HashMap<K,V>& operator= (const HashMap& that)
    {
        size = that.size;
        elements_number = 0;
        table.resize(size, nullptr);

        for(auto it = that.begin(); it != that.end(); ++it)
        {
            this->add(it->key, it->value);
        }

        return *this;
    }

    void check_resize()
    {
        // Если процент заполненности матрицы достигнут - перестроить матрицу!
        if( (double) elements_number >= (double) size * sparsity_percentage)
        {
            // создаём новую таблицу вдвое большего размера
            HashMap<K,V> new_table(this->size * 2);

            // переносим списки в новый массив // рехэшаем
            for(auto it = begin(); it != end(); ++it)
            {
                new_table.add(it->key, it->value);
            }

            *this = new_table;
        }
    }

    int find_uniq_el_number()
    {
        HashMap<V,int> uniq_el_table(size); // вспомогательная таблица для определения уникальных values

        for(auto it = begin(); it != end(); ++it)
        {
            uniq_el_table.add(it->value, 666);
        }

        return uniq_el_table.get_el_number();
    }

    ~HashMap()
    {
        for(auto list : table)
        {
            if(!list) continue;
            auto ptr = list;
            while(ptr)
            {
                auto remove_ptr = ptr;
                ptr = ptr->next;
                delete remove_ptr;
            }
        }
    }
};

// МУЛЬХЭШТАБЛИЦА - ПОДДЕРЖИВАЕТ ХРАНЕНИЕ ЭЛЕМЕНТОВ С ОДИНАКОВЫМИ КЛЮЧАМИ
// поиск элемента по ключу возвращает любой элемент коллекции с заданным ключом - yes - find()
// удаление по заданному ключу удаляет все элементы коллекции с таким ключом - yeas - remove()
// получить все элементы с заданным ключом - yeas - get_all_elements_with_this_key()
// посчитать количество элементов с заданным ключом - yeas
template <typename K, typename T>
class [[maybe_unused]] MultiHashMap
{
private:

    // реализуем собственные стандратные классы list и vector
    // Односвязный список для обхода коллизий методом цепочек - List
    class List
    {
    public:
        K key;
        T value;
        List *next;

        List(K& key, T& value) : key(key), value(value), next(nullptr) {}
    };

    Vector<List*> table; // массив из указателей на односвязные списки - хэштаблица
    int size = 0; // размер таблицы
    int elements_number = 0; // количество всех элементов в таблице
    double sparsity_percentage = 0.7; // процент разряженности таблицы (для данной 70 процентов)

public:
    List* operator[] (const int index) { return table[index]; }

    int hash_func(K key)
    {
        hash<K> hash_value;
        return hash_value(key) % size;
    }

    class iterator;

    // получить все элементы с заданным ключом - возвращает массив элементов
    [[maybe_unused]] void get_all_elements_with_this_key(K key, Vector<T>& elements_arr)
    {
        Vector<T> tmp;
        int index = hash_func(key);
        int i = 0;
        iterator element = iterator(table[index], table.begin() + index, table.end());
        // итератор указывают на ту ячейку массива, где расположен список, содержащий нашу пару (ключ, значение)
        // значит двигаемся по списку пока не нарвёмся на конец списка
        while(element != end())
        {
            // если мы нашли элемент с нужным ключём - добавляем его значение в наш массив
            if(element->key == key)
            {
                if(i >= elements_arr.get_size())
                {
                    tmp = elements_arr;
                    elements_arr.resize(2*elements_arr.get_size());
                    for(int j = 0; j < tmp.get_size(); j++)
                    {
                        elements_arr[j] = tmp[j];
                    }
                }
                elements_arr[i] = element->value;
                i++;
            }
            ++element;
        }
    }

    // посчитать количество элементов с данным ключом
    [[maybe_unused]] int count_elements_number_with_this_key(K key)
    {
        int index = hash_func(key);
        int count = 0;
        iterator element = iterator(table[index], table.begin() + index, table.end());
        // итератор указывают на ту ячейку массива, где расположен список, содержащий нашу пару (ключ, значение)
        // значит двигаемся по списку пока не нарвёмся на конец списка
        while(element != end() && element.get_it() != nullptr)
        {
            // если мы нашли элемент с нужным ключём - увеличиваем счётчик на 1
            if(element->key == key)
                count++;
            ++element;
        }
        return count;
    }

    // та же функция: возвращает итератор на любой элемент данного ключа - первый попавшийся
    iterator find(K& key)
    {
        int index = hash_func(key);
        // если ключа нет вернуть нулевой итератор
        if(!table[index]) return end();

        // итератор на текущий элемент в списке на текущий элемент в массиве
        iterator element = iterator(table[index], table.begin() + index, table.end());
        auto here_vec_it = element.get_vec_it(); //

        // двигаемся по хэш-таблице - ищем элемент под данным ключём
        while(true)
        {
            if(element.get_vec_it() != here_vec_it) return end(); // двигаемся по конкретной ячейке массива
            if(element->key == key) return element; // нашли нужный ключ, возвращаем найденный итератор-элемент
            ++element;
        }
    }

    // функция модифицирована - значения не затираются, а добавляются в самое начало списка
    [[maybe_unused]] void add(K key, T value)
    {
        check_resize(); // проверка хэш-таблицы на разреженность, а уже только потом находим индекс
        // создаём хэш значение и индекс для ключа
        int index = hash_func(key);
        // создаём новую вершину списка и инициализируем его данными значениями key and value
        // иначе если не нулевой создаём новый список, инициализируем и устанавливаем следующим после него
        // список в ячейке index, то есть добавляем узелок в самое начало
        auto node = new List(key, value);
        node->next = table[index];
        table[index] = node;
        elements_number++;
    }

    // удаляет все элементы с ключём key - метод видоизменён (WORK)
    [[maybe_unused]] void remove(K key)
    {
        iterator element = find(key);
        // если указатель на текущий список пуст - завершаем процесс (нечего удалять потому что)
        while (element != end())
        {
            // если существует предыдущий и текущий элемент в списке, то prev.next = it.next (prev -> it -> it.next)
            if (element.get_prev())
                element.get_prev()->next = element->next;
                // иначе если предыдущего узелка нет, устанавливаем it.next следующий после it в самое начала ячейки
            else
                *(element.get_vec_it()) = element->next;
            delete element.get_it();
            elements_number--; // счётчик количества элементов
            element = find(key);
        }
    }

    using vec_iter = typename Vector<List*>:: iterator;

    // Служебный класс Iterator для перемещения по хэш-таблице - тот же
    class iterator
    {
    private:
        List* it; // Указатель на текущий узелок списка
        List* prev; // указатель на предыдущий узелок списка
        vec_iter vec_it; // указатель на текущий элемент массива
        vec_iter vec_end; // указатель на конец массива

    public:
        iterator(List* it, vec_iter vec_it, vec_iter vec_end)
        {
            this->it = it;
            this->vec_it = vec_it;
            this->vec_end = vec_end;
            this->prev = nullptr;
        }

        iterator(const iterator& that): it(that.it), vec_it(that.vec_it), vec_end(that.vec_end), prev(that.prev) {}

        iterator &operator++()
        {

            // движение по списку
            if(it->next)
            {
                prev = it;
                it = it->next;
                return *this;
            }
            prev = nullptr;

            // движение по массиву, пока текущий указатель не станет указателем на конец массива
            vec_it++;
            while(vec_it != vec_end)
            {
                // если arr_it не пусто, то
                if(*vec_it)
                {
                    it = *vec_it;
                    return *this;
                }
                // иначе двигаемся дальше по массиву
                vec_it++;
            }
            it = nullptr;
            return *this;
        }

        bool operator==(const iterator& that) const
        {
            return ((it == that.it) && (vec_it == that.vec_it) && (vec_end == that.vec_end));
        }

        bool operator!= (const iterator& that) const { return !(*this == that); }
        List* operator-> () { return it; }

        List& operator* () { return *it; }
        List* get_prev () { return prev; }
        List* get_it () { return it; }
        vec_iter get_vec_it() { return vec_it; }
    };

    iterator begin() const {
        for(vec_iter it = table.begin(); it != table.end() ; it++)
        {
            if(*it) return iterator(*it , it , table.end());
        }
        return end();
    }

    iterator end() const { return iterator(nullptr, table.end(), table.end()); }

    [[maybe_unused]] [[nodiscard]] int get_el_number() const { return elements_number; }

    [[maybe_unused]] explicit MultiHashMap(int size)
    {
        this->size = size;
        table.resize(size, nullptr);
        elements_number = 0;
    }

    // конструктор копирования
    MultiHashMap(MultiHashMap& map)
    {
        this = map;
    }

    [[maybe_unused]] void print_hashtable() const
    {
        for(auto it = begin(); it != end(); ++it)
        {
            cout << it->key << " " << it->value << endl;
        }
    }

    // нужно всё хорошо скопировать, размер таблицы(количество списков), сумарное количество вершин у списков,
    // увеличить сам массив списков (или уменьшить), и все узелки скопировать в новую талицу
    MultiHashMap<K,T>& operator= (const MultiHashMap& that)
    {

        size = that.size;
        elements_number = 0;
        table.resize(size, nullptr);

        for(auto it = that.begin(); it != that.end(); ++it)
        {
            this->add(it->key, it->value);
        }

        return *this;
    }

    void check_resize()
    {
        // Если процент заполненности матрицы достигнут - перестроить матрицу!
        if( (double) elements_number >= (double) size * sparsity_percentage)
        {
            // создаём новую таблицу вдвое большего размера
            MultiHashMap<K,T> new_table(this->size * 2);

            // переносим списки в новый массив // рехэшаем
            for(auto it = begin(); it != end(); ++it)
            {
                new_table.add(it->key, it->value);
            }

            *this = new_table;
        }
    }

    // функция определяет количество уникальных элементов в хэш-таблице
    [[maybe_unused]] int find_uniq_el_number()
    {
        int count = 0;
        for(auto it = begin(); it != end(); ++it)
        {
            // Вынесем текущий элемент в переменную
            T currentElementValue = it->value;
            // Имеет ли текущий элемент значение, встречающееся в массиве
            // по умолчанию нет
            bool hasEquals = false;

            /* Берем элемент и ищем по массиву такое же значение */
            for (auto r = begin(); r != it; ++r) {
                // С самим с собой не сравниваем, пропускаем итерацию
                if (it == r) { continue; }
                // Если текущий элемент равен другому элементу
                if (currentElementValue == r->value && it != r) {
                    // То устанавливаем флаг, что такое значение встречалось
                    hasEquals = true;
                }
            }
            // Если у этого элемента нет встречающихся значений, то счётчик обновляется
            if (!hasEquals) {
                count++;
            }
        }

        return count;
    }

    // деструктор тот же
    ~MultiHashMap()
    {
        for(auto list : table)
        {
            if(!list) continue;
            auto ptr = list;
            while(ptr)
            {
                auto remove_ptr = ptr;
                ptr = ptr->next;
                delete remove_ptr;
            }
        }
    }
};

template <typename K , typename V>
void do_work()
{
    char command;
    int command_number;
    K key;
    V value;
    cin >> command_number;
    HashMap<K,V> hashTable(2);
    for(int i = 0; i < command_number; i++)
    {
        cin >> command;
        switch(command)
        {
            case 'A':
                cin >> key >> value;
                hashTable.add(key, value);
                break;
            case 'R' :
                cin >> key;
                hashTable.remove(key);
                break;
            default:
                cout << "Wrong letter!!!" << endl; exit(2);
        }
    }

    int uniq_el_number = hashTable.find_uniq_el_number();
    cout << hashTable.get_el_number() << " " << uniq_el_number << endl;

    // extra
//    cout << "Type a key to get elements' number with this key" << endl;
//    cin >> key;
//    cout << hashTable.count_elements_number_with_this_key(key) << endl;
//    cout << "Type a key to get elements with this key" << endl;
//    cin >> key;
//    Vector<V> uniq_arr{1};
//    hashTable.get_all_elements_with_this_key(key, uniq_arr);
//    for(int i = 0; i < uniq_arr.get_size(); i++)
//    {
//        cout << uniq_arr[i] << " ";
//    }
//    cout << endl;
}

int main()
{
    char type_key, type_value;
    cin >> type_key >> type_value;
    switch(type_key)
    {
        case 'S':
            if (type_value == 'S') do_work<string, string>();
            if (type_value == 'I') do_work<string, int>();
            if (type_value == 'D') do_work<string, double>();
            break;
        case 'I':
            if (type_value == 'S') do_work<int, string>();
            if (type_value == 'I') do_work<int, int>();
            if (type_value == 'D') do_work<int, double>();
            break;
        case 'D':
            if (type_value == 'S') do_work<double, string>();
            if (type_value == 'I') do_work<double, int>();
            if (type_value == 'D') do_work<double, double>();
            break;
        case 'M': // M - Matrix ((size - elements) are a key)
            if (type_value == 'S') do_work<Matrix, string>();
            if (type_value == 'I') do_work<Matrix, int>();
            if (type_value == 'D') do_work<Matrix, double>();
            break;
        default: cout << "Wrong letter!!!"; exit(1);
    }
    return 0;
}