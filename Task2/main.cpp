#include <iostream>
#include <vector>

using namespace std;

enum Direction
{
    Down = 0,
    Right,
    Up,
    Left
};

class Animal
{
protected:
    int x, y; // координаты зверя
    int direction; // направление, куда он собирается двигаться (вверх, вниз, вправо, влево)
    int constancy; // постоянство - раз в сколько ходов зверь захочет поменять направление своего движения
    int temp_constancy; // количество шагов до смены направления
    int age; // возраст - сколько ходов зверь уже существует. Изначально возраст зверя равен нулю.

public:
    virtual bool is_rabbit() = 0;
    virtual void feed(vector<Animal*> &animals) = 0;
    virtual void breeding(vector<Animal*> &animals) = 0;
    virtual bool dying() = 0;

    virtual ~Animal() = default;

    void move(int N, int M)
    {
        int step;
        if(this->is_rabbit()) { step = 1; }
        else { step = 2; }

        if (direction == Up) y += step;
        else if (direction == Right) x += step;
        else if (direction == Down) y -= step;
        else if (direction == Left) x -= step;

        temp_constancy--;
        if (temp_constancy == 0)
        {
            if (direction == Left) direction = Up;
            else direction++;
        }

        if (this->get_x() < 0) this->set_x(N + this->get_x());
        else if (this->get_x() >= N) this->set_x(this->get_x() - N);
        if (this->get_y() < 0) this->set_y(M + this->get_y());
        else if (this->get_y() >= M) this->set_y(this->get_y() - M);
    }

    Animal(int x, int y, int direction, int constancy)
    {
        this-> x = x;
        this-> y = y;
        this-> direction = direction;
        this-> constancy = constancy;
        temp_constancy = constancy;
        this-> age = 0;
    }

    [[nodiscard]] int get_x() const
    {
        return x;
    }

    [[nodiscard]] int get_y() const
    {
        return y;
    }

    [[nodiscard]] int get_age() const
    {
        return age;
    }

    [[nodiscard]] int get_direction() const
    {
        return direction;
    }

    [[nodiscard]] int get_constancy() const
    {
        return constancy;
    }

    void set_x(int _x)
    {
        this->x = _x;
    }

    void set_y(int _y)
    {
        this->y = _y;
    }

    void set_age(int _age)
    {
        this->age = _age;
    }

};

class Wolf: public Animal
{
private:
public:
    int ate_rabbits = 0; // количество съеденных зайцев
    Wolf(int x, int y, int direction, int constancy) : Animal(x, y, direction, constancy) {}

    ~Wolf() override = default;

    bool is_rabbit() override
    {
        return false;
    }

    void feed(vector<Animal*> &animals) override
    {
        for(auto & animal : animals)
        {
            // смотрим есть ли на координатах зайца какойнибуть волк, если да то идём дальше
            if(animal != nullptr && animal->is_rabbit())
            {
                if (this->get_y() == animal->get_y() &&
                    this->get_x() == animal->get_x())
                {
                    // съедаем этого зайца и записываем его в зачёт
                    this->ate_rabbits++;
                    delete animal;
                    animal = nullptr;
                }
            }
        }
    }

    void breeding(vector<Animal*> &animals) override
    {
        if(this->ate_rabbits > 1)
        {
            animals.push_back(new Wolf(this->get_x(), this->get_y(),
                                       this->get_direction(), this->get_constancy()));
            this->ate_rabbits = 0;
        }
    }

    bool dying() override
    {
        if (this->get_age() == 15) // если условие тру то волк умирает
        {
            return true;
        }
        return false;
    };
};

class Rabbit: public Animal
{
public:
    Rabbit(int x, int y, int direction, int constancy) : Animal(x, y, direction, constancy) {}
    ~Rabbit() override = default;

    bool is_rabbit() override
    {
        return true;
    }

    void feed(vector<Animal*> &animals) override {} // заяц питается святым духом
    void breeding(vector<Animal*> &animals) override
    {
        if (this->get_age() == 10 || this->get_age() == 5)
        {
            animals.push_back(new Rabbit(this->get_x(), this->get_y(),
                                         this->get_direction(), this->get_constancy()));
        }
    }
    bool dying() override
    {
        if (this->get_age() == 10) // если условие тру то rabbit умирает
        {
            return true;
        }
        return false;
    };
};

// гиена ест всех подряд и даже других гиен (сперва есть самых старых)
// после двух съеденных животных, они наедаются и больше не едят
// в остальном во всём схожи с волками
class Hyena : public Animal
{
private:
    bool coward = false;
public:

    int ate_animals = 0;
    Hyena(int x, int y, int direction, int constancy) : Animal(x, y, direction, constancy) {}

    ~Hyena() override = default;

    bool is_rabbit() override
    {
        return false;
    }

    void feed(vector<Animal*> &animals) override
    {
        for(auto & animal : animals)
        {
            if(animal != nullptr && !this->coward) // there is something and hasn't eaten 2 animals yet
            {
                // смотрим нет есть ли на координатах зайца или волка или гиены какойнибуть гиены, если да то идём дальше
                if (this->get_y() == animal->get_y() &&
                    this->get_x() == animal->get_x())
                {
                    if (this != animal) // can't eat themselves
                    {
                        // съедаем это животное ии записываем его в зачёт
                        this->ate_animals++;
                        delete animal;
                        animal = nullptr;

                        if(this->ate_animals == 2) // если гиена съела больше 2ух животных, то становится трусом
                        {
                            this->coward = true;
                        }
                    }
                }
            }
        }
    }

    void breeding(vector<Animal*> &animals) override
    {
        if(this->ate_animals > 1)
        {
            animals.push_back(new Wolf(this->get_x(), this->get_y(),
                                       this->get_direction(), this->get_constancy()));
            this->ate_animals = 0;
        }
    }

    bool dying() override
    {
        if (this->get_age() == 15) // если условие тру то гиена умирает
        {
            return true;
        }
        return false;
    };
};

class Simulation
{
    int N, M; // размеры поля
    int steps; // количество ходов
    int** field; // поле
    vector<Animal*> animals;

public:

    Simulation(int N, int M, int steps, vector<Animal*>& animals)
    {
        this-> N = N;
        this-> M = M;
        this-> steps = steps;
        field = new int*[N];
        for (int i = 0; i < N; ++i)
        {
            field[i] = new int[M]();
        }
        this->animals = animals;
    }

    ~Simulation()
    {
        for (int j = 0; j < N; ++j)
            delete[] field[j];
        delete[] field;

        for(auto & animal : animals)
            if(animal != nullptr) delete animal;
    }

    void start()
    {
        for (int i = 0; i < steps; i++)
        {
            for(int j = 0; j < animals.size(); j++)
            {
                if(animals[j] != nullptr)
                {
                    animals[j]->set_age(animals[j]->get_age() + 1); // животное взрослеет на 1 годик
                    animals[j]->move(N, M); // животное делает ход
                }
            }

            // питание
            for(int j = 0; j < animals.size(); j++)
            {
                if(animals[j] != nullptr)
                {
                    animals[j]->feed(animals);
                }
            }

            // reproduction
            for(int j = 0; j < animals.size(); j++)
            {
                if(animals[j] != nullptr)
                {
                    animals[j]->breeding(animals);
                }
            }

            // dying
            for(auto & animal : animals)
            {
                if(animal != nullptr)
                {
                    if (animal->dying())
                    {
                        delete animal;
                        animal = nullptr;
                    }
                }
            }

            // делаем так чтобы в начале не скапливались nullptr'ы, а потом изменяем размер массива на поменьше
            int step = 0;
            for(int j = 0; j < animals.size(); j++)
            {
                if(animals[j] == nullptr) step++;
                else if (step != 0)
                {
                    animals[j - step] = animals[j];
                }
            }
            animals.resize(animals.size() - step);
        }
    }

    void print()
    {
        for(int i = 0; i < animals.size(); i++)
        {
            if(animals[i] != nullptr)
            {
                if (animals[i]->is_rabbit())
                {
                    field[animals[i]->get_y()][animals[i]->get_x()]++;
                }
                else
                {
                    field[animals[i]->get_y()][animals[i]->get_x()]--;
                }
            }
        }

        for (int i = 0; i < M; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (field[i][j] == 0) cout << '#';
                else cout << field[i][j];
            }
            cout << endl;
        }
    }
};

int main() {
    int field_width, field_length;
    int steps;
    cin >> field_width >> field_length >> steps; // ширина и длинна поля, количество шагов

    int wolves_count, rabbits_count;
    cin >> rabbits_count >> wolves_count; // кол-во зайцев и волков
//    int hyena_count;
//    cin >> hyena_count;

    vector<Animal*> animals;

    int x, y, start_direction, constancy;

    for (int i = 0; i < rabbits_count; ++i) // инициализируем зайцев
    {
        cin >> x >> y >> start_direction >> constancy;
        animals.push_back(new Rabbit(x, y, start_direction, constancy));
    }
    for (int i = 0; i < wolves_count; ++i) // инициализируем волков
    {
        cin >> x >> y >> start_direction >> constancy;
        animals.push_back(new Wolf(x, y, start_direction, constancy));
    }
//    for (int i = 0; i < hyena_count; ++i) // инициализируем гиен
//    {
//        cin >> x >> y >> start_direction >> constancy;
//        animals.push_back(new Hyena(x, y, start_direction, constancy));
//    }

    // запуск симуляции
    Simulation SIM(field_width, field_length, steps, animals);
    SIM.start();
    SIM.print();

    return 0;
}