#include <iostream>

using namespace std;

enum Orient // Создан для удобства
{
    row = 1,
    column = 2,
};

void index_error()
{
    cout << "Incorrect index";
    exit(1);
}

void dimension_error() // сообщение об ошибке при несовпадении размерностей матриц
{
    cout << "Matrices' dimensions don't match";
    exit(1);
}

class Vector
{
private:

    int** v_vector;
    int v_length;
    int v_index;
    Orient v_orient;

public:

    explicit Vector(int length, Orient orient)
    {
        v_length = length;
        v_orient = orient;
        v_index = 0;
        v_vector = new int*[v_length];
        for(int i = 0; i < v_length; i++)
        {
            v_vector[i] = nullptr;
        }
    }

    Vector(const Vector& vector) // конструктор копирования
    {
        v_length = vector.v_length;
        v_orient = vector.v_orient;
        v_index = vector.v_index;
        v_vector = new int* [v_length];
        for(int i = 0; i < v_length; i++)
        {
            v_vector[i] = vector.v_vector[i];
        }
    }

    Vector& operator= (const Vector& vector) = delete;
    friend std::ostream& operator<< (std::ostream &out, const Vector &vector);
    int& operator[] (int index);
    friend class Matrix;

    ~Vector()
    {
        delete[] v_vector;
    }
};

int& Vector:: operator[] (int index) // доступ по индексу
{
    if(index >= v_length)
    {
        index_error();
    }
    v_index = index;
    return *v_vector[v_index];
}

std::ostream& operator<< (std::ostream &out, const Vector &vector) // вывод вектор-столбец или вектор-строка
{
    if (vector.v_orient == row)
    {
        for (int j = 0; j < vector.v_length; j++)
        {
            out << *vector.v_vector[j] << " ";
        }
    }
    else
    {
        for (int j = 0; j < vector.v_length; j++)
        {
            out << *vector.v_vector[j] << endl;
        }
    }
    return out;
}

class Matrix
{
private:

    int** m_matrix; // квадратная матрица
    int m_size; // её размер

public:

    Matrix() // Конструктор по умолчанию для инициализации матрицы размерности 0
    {
       m_matrix = nullptr;
       m_size = 0;
    }

    explicit Matrix(int size) // Конструктор инициализирует единичную матрицу
    {
        m_size = size;
        m_matrix = new int* [m_size];
        for(int j = 0; j < m_size; j++)
        {
            m_matrix[j] = new int[m_size]();
            m_matrix[j][j] = 1;
        }
    }

    Matrix(int size, int k) // Конструктор на глав диагонале значение k
    {
        m_size = size;
        m_matrix = new int* [m_size];
        for(int j = 0; j < m_size; j++)
        {
            m_matrix[j] = new int[m_size]();
            m_matrix[j][j] = k;
        }
    }

    [[maybe_unused]] Matrix(int size, int const* array) // конструктор на глав диагонале элементы из массива array
    {
        m_size = size;
        m_matrix = new int* [m_size];
        for(int j = 0; j < m_size; j++)
        {
            m_matrix[j] = new int[m_size]();
            m_matrix[j][j] = array[j];
        }
    }

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

    friend std::ostream& operator<< (std::ostream &out, const Matrix &matrix);
    friend std::istream& operator>> (std::istream &in, Matrix &matrix);
    friend bool operator== (const Matrix &matrix1, const Matrix &matrix2);
    friend bool operator!= (const Matrix &matrix1, const Matrix &matrix2);
    Matrix operator+ (const Matrix& matrix) const;
    Matrix operator- (const Matrix& matrix) const;
    Matrix operator* (const Matrix& matrix) const;
    Matrix operator! ();
    Matrix operator() (int row, int column);
    friend void swap(Matrix& matrix1, Matrix& matrix2);
    Matrix& operator= (const Matrix& matrix);
    Vector operator() (int column);
    Vector operator[] (int row);

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

Matrix Matrix:: operator+ (const Matrix& matrix) const // сложение двух матриц
{
    Matrix rez{m_size, 0};
    if(m_size != matrix.m_size)
    {
        dimension_error();
    }
    for(int i = 0; i < m_size; i++)
    {
        for(int j = 0; j < m_size; j++)
        {
            rez.m_matrix[i][j] = m_matrix[i][j] + matrix.m_matrix[i][j];
        }
    }
    return rez;
}

Matrix Matrix:: operator- (const Matrix& matrix) const // оператор вычитания двух матриц
{
    Matrix rez{m_size, 0};
    if(m_size != matrix.m_size)
    {
        dimension_error();
    }
    for(int i = 0; i < m_size; i++)
    {
        for(int j = 0; j < m_size; j++)
        {
            rez.m_matrix[i][j] = m_matrix[i][j] - matrix.m_matrix[i][j];
        }
    }
    return rez;
}

Matrix Matrix:: operator* (const Matrix& matrix) const // умножение двух матриц
{
    Matrix rez{m_size, 0};
    if(m_size != matrix.m_size)
    {
        dimension_error();
    }
    for(int i = 0; i < m_size; i++)
    {
        for(int j = 0; j < m_size; j++)
        {
            for(int k = 0; k < m_size; k++)
            {
                rez.m_matrix[i][j] += m_matrix[i][k] * matrix.m_matrix[k][j];
            }
        }
    }
    return rez;
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

bool operator!= (const Matrix &matrix1, const Matrix &matrix2) // неравенство двух матриц
{
    return !(matrix1 == matrix2);
}

Matrix Matrix:: operator! () // оператор транспонирования матрицы
{
    Matrix rez{m_size, 0};
    for (int i = 0; i < m_size; ++i)
    {
        for (int j = 0; j < m_size; ++j)
        {
            rez.m_matrix[i][j] = m_matrix[j][i];
        }
    }
    return rez;
}

Matrix Matrix:: operator() (int row, int column) // оператор взятия минора
{
    Matrix rez{m_size - 1, 0};
    for (int i = 0; i < m_size; ++i)
    {
        for (int j = 0; j < m_size; ++j)
        {
            if (i < row && j < column) rez.m_matrix[i][j] = m_matrix[i][j];
            else if (i > row && j < column)  rez.m_matrix[i - 1][j] = m_matrix[i][j];
            else if (i < row && j > column)  rez.m_matrix[i][j - 1] = m_matrix[i][j];
            else if (i > row && j > column)  rez.m_matrix[i - 1][j - 1] = m_matrix[i][j];
        }
    }
    return rez;
}

void swap(Matrix& matrix1, Matrix& matrix2)
{
    swap(matrix1.m_size, matrix2.m_size);
    swap(matrix1.m_matrix, matrix2.m_matrix);
}

Matrix& Matrix:: operator= (const Matrix& matrix) {
    Matrix temp{matrix};
    swap(*this, temp);
    return *this;
}

Vector Matrix:: operator() (int column_index) // оператор взятия столбца
{
    if(column_index >= m_size)
    {
        index_error();
    }
    Vector m_column{m_size, column};
    for(int i = 0; i < m_size; i++)
    {
        m_column.v_vector[i] = &m_matrix[i][column_index];
    }
    return m_column;
}

Vector Matrix:: operator[] (int row_index) // оператор взятия строки
{
    if(row_index >= m_size)
    {
        index_error();
    }
    Vector m_row{m_size, row};
    for(int i = 0; i < m_size; i++)
    {
        m_row.v_vector[i] = &m_matrix[row_index][i];
    }
    return m_row;
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
    for(int i = 0; i < matrix.m_size; i++)
    {
        for(int j = 0; j < matrix.m_size; j++)
        {
            in >> matrix.m_matrix[i][j];
        }
    }
    return in;
}

int main() {
    int N, k; // Размерности матриц N и число на диагонале k
    cin >> N;
    cin >> k;

    Matrix A{N}, B{N}, C{N}, D{N}, K{N, k}, result{N};
    cin >> A >> B >> C >> D;

    result = ((A + B * !C + K) * !D);
    cout << result;

    // Для показательного выступления доп. задания - раскомментировать (матрица 3на3)!!!
    // Работаем со столбиками
//    cout << endl << "OLD:" << endl << D;
//    cout << "D(1):" << endl << D(1) << "D(1)[0]: " <<  D(1)[0] << endl << "D(1)[1]: " << D(1)[1] << endl <<
//                                           "D(1)[2]: " << D(1)[2] << endl;
//    D(1)[0] = 555;
//    D(1)[1] = 555;
//    D(1)[2] = 555;
//    cout << endl << "NEW:" << endl << D;
//    cout << "D(1):"<< endl << D(1) << "D(1)[0]: " <<  D(1)[0] << endl << "D(1)[1]: " << D(1)[1] << endl <<
//                                           "D(1)[2]: " << D(1)[2] << endl;
    // Заканчиваем работать со столбиками
    // Начинаем работать со строками
//    cout << endl << "OLD:" << endl << D;
//    cout << "D[1]:" << D[1] << endl << "D[1][0]: " <<  D[1][0] << endl << "D[1][1]: " << D[1][1] << endl <<
//         "D[1][2]: " << D[1][2] << endl;
//    D[1][0] = 666;
//    D[1][1] = 666;
//    D[1][2] = 666;
//    cout << endl << "NEW:" << endl << D;
//    cout << "D[1]:" << D[1] << endl << "D[1][0]: " <<  D[1][0] << endl << "D[1][1]: " << D[1][1] << endl <<
//         "D[1][2]: " << D[1][2] << endl;
    // Заканчиваем работать со строками

    return 0;
}