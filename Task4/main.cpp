// vladdd

#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>

using namespace std;

// transition func class
class transition
{
private:
    int from;
    int to;
    char symbol;

public:
    transition(int from, int to, char symbol)
    {
        this->from = from;
        this->to = to;
        this->symbol = symbol;
    }

    transition()
    {
        this->from = 0;
        this->to = 0;
        this->symbol = 0;
    }

    [[nodiscard]] char get_symbol() const
    {
        return symbol;
    }

    [[nodiscard]] int get_from() const
    {
        return from;
    }

    [[nodiscard]] int get_to() const
    {
        return to;
    }
};

// dfa consists of arrays of finals states, of transition's functions, states and start state
// class finite automaton
// в детерминированном автомате из каждой вершины по каждому входному символу определен переход в точности в одну вершину
class FA
{
private:
    vector<int> states;
    vector<int> final_states; // массив конечных состояний
    vector<transition> functions; // массив функций перехода
    int start_state = 0; // начальное состояние
    int states_number = 1; // общее количество состояний
    int final_state = 0; // для удобства построения нка с одним начальным состоянием
    int dead_state = -1;

public:

    FA(vector<int> &final_states , vector<transition>& functions, int start_state, int states_number)
    {
        this->final_states = final_states;
        this->functions = functions;
        this->start_state = start_state;
        this->states_number = states_number;
    }

    FA()
    {
        this->start_state = 0;
        this->states_number = 1;
    }

    FA & operator= (const FA& fa) = default;

    [[nodiscard]] int get_start_state() const { return start_state; }

    // функция находим множества состояний, в которое может перейти состояние state по конкретной букве
    vector<int> find_subset(vector<int>& state, char letter)
    {
        set<int> subset;
        vector<int> final_subset;
        // для каждого состояния из множества states ищем в какие состояния они переходят по символу letter
        for(int j = 0; j < state.size(); j++)
        {
            for (int i = 0; i < functions.size(); i++)
            {
                if (functions[i].get_from() == state[j] && functions[i].get_symbol() == letter)
                {
                    subset.insert(functions[i].get_to());
                }
            }
        }
        for(auto it = subset.begin(); it != subset.end(); it++)
        {
            final_subset.push_back(*it);
        }
        return final_subset;
    }

    static bool is_subset_uniq(vector<vector<int>>& subset_array, vector<int>& subset)
    {
        for(int j = 0; j < subset_array.size(); j++)
        {
            if(subset == subset_array[j])
            {
                return false;
            }
        }
        return true;
    }

    static int get_state_index(vector<vector<int>>& subset_array, vector<int>& subset)
    {
        for(int j = 0; j < subset_array.size(); j++)
        {
            if(subset == subset_array[j])
            {
                return j;
            }
        }
        cout << "It must not be happened. BIG ERROR";
        return -1;
    }

    // используется так называемый метод подмножеств (или метод Томпсона)
    FA NFA_to_DFA(string letters)
    {
        set<int> new_final_states; // контейнер для хранения конечных состояний нового КА
        FA new_nfa{}; // инициализируем новый автомат, который будет детерминированным
        queue<vector<int>> Q; // очередь, в которой будут лежать множества состояний перехода
        vector<int> subset{}; // массив, хрянящий множества состояний перехода
        vector<vector<int>> subset_array; // в нём будет хранить все найденные уникальные подмножества
        subset.push_back(start_state); // первым в очередь идёт начальное состояние, т.е массив состоит только из 1 состояния
        subset_array.push_back(subset); // добавим первое подмножество состояний в множество уникальных подмножеств состояний
        Q.push(subset); // запихиваем его в очередь первым

        int to_state;
        for(int end_state : final_states) // вдруг начальное состояние также является конечным.
        {
            if(start_state == end_state)
            {
                new_final_states.insert(start_state);
            }
        }

        int from_state = 0; // в новом автомате начальным состоянием будет 0
        // пока в очереди ктото есть - процесс продолжается
        while(!Q.empty())
        {
            vector<int> temp1 = Q.front(); // достаём массив состояний из очереди в кладём его в temp1

            Q.pop(); // достаем из очереди массив состояний
            // ищем множества состояний to для каждого состояния from из подмножества temp1
            for(int i = 0; i < letters.size(); i++)
            {
                subset = this->find_subset(temp1, letters[i]); // нашли подмножество - запихали его в очередь
                // проверка, а не было ли такого подмножества раньше? если не было - добавляем в очередь
                if(is_subset_uniq(subset_array, subset))
                {
                    Q.push(subset);
                    subset_array.push_back(subset); // а также добовляем в массив уникальных подмножеств
                }
                to_state = get_state_index(subset_array, subset); // находим номер состояния, в который переходит from
                new_nfa.insert(from_state, to_state, letters[i]); // добавили функцию перехода
                new_nfa.states_number++; // увеличили количество состояний в новом автомате на 1
                // будет ли to_state конечным?

                for(int j = 0; j < subset.size(); j++)
                {
                    for(int k = 0; k < final_states.size(); k++)
                    {
                        if (subset[j] == final_states[k])
                        {
                            new_final_states.insert(to_state);
                        }
                    }
                }
            }
            from_state++;
        }

        // добовляем их всех в новый автомат
        for(auto it = new_final_states.begin(); it != new_final_states.end(); it++)
        {
            new_nfa.final_states.push_back(*it);
        }

        // также нужно задианонить мертвое состояние (должно быть одно), чтобы thread_check норм работал
        // (хотя как выяснилось оно и так работает)
        bool flag = true;
        for(int i = 0; i < new_nfa.states.size(); i++)
        {
            for(int j = 0; j < new_nfa.functions.size(); j++)
            {
                // если вершина не ведёт ни куда кроме как в саму себя то это мертвая вершина
                // ищем в массиве функций переходы именно состояния i, если if ни разу не выполнился, то i мертвое состояние
                if(states[i] == new_nfa.functions[j].get_from() && new_nfa.functions[j].get_from() != new_nfa.functions[j].get_to())
                {
                    flag = false;
                }
            }
            if(!flag)
            {
                new_nfa.dead_state = states[i];
                continue;
            }
        }

        return new_nfa;
    }

    void add_state(int state)
    {
        states.push_back(state);
    }

    void set_all_states(int no_states)
    {
        for(int i = 0; i < no_states; i++)
        {
            states.push_back(i);
        }
    }

    void insert(int from, int to, char symbol)
    {
        transition trans{from, to, symbol};
        functions.push_back(trans);
    }

    // операция звёздочка Клини - приминяем к автомату
    FA Kleene(FA fa)
    {
        FA result;
        transition new_trans;

        result.set_all_states(fa.states_number + 2); // добавляем состояния в массив состояний
        result.states_number = fa.states_number + 2; // устанавливаем количество состояний в новом автомате
        result.insert(0, 1, '^'); // первый переход в новом автомате - эпсилон переход

        // идём по всем фунцкиям автомата fa - добовляем их в новый автомат
        for(int i = 0; i < fa.functions.size(); i++)
        {
            new_trans = fa.functions[i];
            result.insert(new_trans.get_from() + 1, new_trans.get_to() + 1,
                          new_trans.get_symbol());
        }

        result.insert(fa.states_number, fa.states_number + 1, '^'); // эпсилон переход в новое конечное состояние
        // - обеспечивает пустой результат звёздочки клини
        result.insert(fa.states_number, 1, '^'); // эпсилон переход из предпоследнего состояния в предначальное
        result.insert(0, fa.states_number + 1, '^'); // эпсилон переход из начального состояние в конечное - когда клини - это ноль

        result.final_state = fa.states_number + 1; // финал. состояние результ. автомата это последнее состояние на эпсилон переходе

        return result;
    }

    // КОНКАТЕНАЦИЯ - из стэка взяли два автомата между которыми операция конкатенации
    // Построим по ним общий автомат
    FA Concat(FA fa1, FA fa2)
    {
        FA result;
        result.set_all_states(fa1.states_number + fa2.states_number);
        result.states_number = fa1.states_number + fa2.states_number; // количество состояний результирующего автомата равно сумме двух входных

        transition new_trans;
        // проходимся по всем функциям автомата f1 и добовляем их в результирующий
        for(int i = 0; i < fa1.functions.size(); i++)
        {
            new_trans = fa1.functions[i];
            result.insert(new_trans.get_from(), new_trans.get_to(), new_trans.get_symbol());
        }

        // между ними будет эпсилон переход
        result.insert(fa1.final_state, fa1.states_number, '^');

        // проходимся по всем функциям автомата f2 и добовляем их в результирующий, учитываем, что мы уже добавили
        // функции до это, в состовлении нумерации
        for(int i = 0; i < fa2.functions.size(); i++)
        {
            new_trans = fa2.functions[i];
            result.insert(new_trans.get_from() + fa1.states_number, new_trans.get_to()
                                               + fa1.states_number, new_trans.get_symbol());
        }

        // конечное состояние второго автомата - конечное состояние рузельтирующего автомата
        result.final_state = fa1.states_number + fa2.states_number - 1;

        return result;
    }

    // Объединение двух автоматов
    FA Union(vector<FA> automatons, int fa_number)
    {
        FA result;
        result.states_number = 2; // так как будет ещё два эпсилон перехода помимо всего прочего

        // считаем количество состояний всех автмоатов + 2 эпсилон перехода
        for(int i = 0; i < fa_number; i++)
        {
            result.states_number += automatons[i].states_number;
        }

        // состояния результирующего автомата - состоянния вех автоматов + 2 состояния по эпсилон переходам
        result.set_all_states(result.states_number);

        int adder_track = 1; // отслеживаем куда идёт переход
        FA temp_fa;
        transition new_trans;

        // из нового начального состояния в каждое из новых автоматов идёт эпсилон переход и из каждого автомата
        // в новое состояние также идёт эпсилон переход
        for(int i = 0; i < fa_number; i++)
        {
            // из начального состояния в каждый новый автомат по эпсилон переходу
            result.insert(0, adder_track, '^');
            temp_fa = automatons[i];
            // берём каждый автомат и проходим по его функциям и добавляем в результ. автомат (внимательно с индексами!)
            for(int j = 0; j < temp_fa.functions.size(); j++)
            {
                new_trans = temp_fa.functions[j];
                result.insert(new_trans.get_from() + adder_track,
                                      new_trans.get_to() + adder_track, new_trans.get_symbol());
            }
            // счётчик состояний увеличиваем на количество добовленных из данного автомата на итой итерации
            adder_track += temp_fa.states_number;
            // и добовляем эпсилон переход из последнего состояния конкретного автомата в новое конечное состояние результ. автомата
            result.insert(adder_track - 1, result.states_number - 1, '^');
        }

        // конечное состояние результ. автомата - новое состояние по эпсилон переходу
        result.final_state = result.states_number - 1;

        return result;
    }

    // Функция организует работу по переводу регулярки в нка с эпсилон переходами
    FA regexp_to_nfa(string re)
    {
        stack<char> operators_stack;
        stack<FA> fa_stack;

        // идём по регулярному выржаению, записанному в строке
        for(string::iterator it = re.begin(); it != re.end(); ++it)
        {
            char cur_sym = *it; // берём символ из рег выражения (current symbol)
            // если это не какой-то служебный символ, то создаём автомат с таким переходом
            if(cur_sym != '(' && cur_sym != ')' && cur_sym != '*'
               && cur_sym != '|' && cur_sym != '.')
            {
                FA new_sym;
                new_sym.set_all_states(2);
                new_sym.insert(0, 1, cur_sym);
                new_sym.states_number = 2;
                new_sym.final_state = 1;
                fa_stack.push(new_sym);
            }
            else
            {
                // иначе если это звёздочка клини, то записываем в star_sym верхний автомат из стека
                // т.е звёздочку клини выполняем сразу же
                if(cur_sym == '*')
                {
                    FA star_sym = fa_stack.top(); // достаём автомат из стека в star_sym
                    fa_stack.pop(); // удаляем автомат из стека
                    fa_stack.push(Kleene(star_sym)); // в очередь идёт автомат после преобразования клини
                }
                // если точка конкатенации или объединение или откр скобка - в стэк операторов
                else if(cur_sym == '.' || cur_sym == '|' || cur_sym == '(')
                {
                    operators_stack.push(cur_sym);
                }
                else
                {
                    int op_count = 0; // количество операторов
                    char op_sym = operators_stack.top(); // берём служебный символ из стека
                    if(op_sym == '(') continue; // если откр скобка то двигаемся - завершаем эту итерацию

                    // пока оператор не равен отк скобке - удаляем верхний элемент
                    do
                    {
                        operators_stack.pop();
                        op_count++;
                    } while(operators_stack.top() != '(');

                    operators_stack.pop();
                    vector<FA> automatons; // массив для объединения

                    // если конкатенация
                    if(op_sym == '.')
                    {
                        // кладем в op1 и op2 из стека два автомата и удаляем из стека и применяем функцию конкатенации для них
                        for(int i = 0; i < op_count; i++)
                        {
                            FA op2 = fa_stack.top();
                            fa_stack.pop();
                            FA op1 = fa_stack.top();
                            fa_stack.pop();
                            // заметим что соединяем два автомата в порядке обратном как мы их доставали из стэка
                            fa_stack.push(Concat(op1, op2));
                        }
                    }
                    // иначе если оператор - объединение, то
                    else if(op_sym == '|')
                    {
                        automatons.assign(op_count + 1, FA()); // заменяем (op_count+1) первых значений автоматами
                        int tracker = op_count;
                        // запихиваем в массив объединения автоматов автоматы из стека, удаляем их от туда
                        for(int i = 0; i < op_count + 1; i++)
                        {
                            automatons[tracker] = fa_stack.top();
                            tracker--;
                            fa_stack.pop();
                        }
                        fa_stack.push(Union(automatons, op_count + 1));
                    }
                }
            }
        }
        return fa_stack.top(); // в итоге последний автомат, оставшийся в стэке - наш нка с eps
    }

    // e-closure is all the states that can be reached from a particular state only by seeing the e symbol
    // e-closure = {e - s - e}
    // найдём множество перехода для состояния state
    // состояние из которго идём, по какой букве, какой черёд, множество куда из state по letter и эпсилон
    void e_closure(int state, char letter, int flag, set<int>& conds)
    {
        for(int i = 0; i < functions.size(); i++)
        {
//            cout << "flag " << flag << endl;
            // нашли нужную функцию с эпсилон переходом (флаг говорит нам что мы ищём эпсилон переход первый раз)
            // при этом это не эпсилон переход из себя в себя
            if(functions[i].get_from() == state && functions[i].get_symbol() == '^' && flag == 1)
            {
                // сверяем содержит ли уже пройденный путь данное состояние, если да то рекурсивно откатываемся на шаг назад
                for(auto it = conds.begin(); it != conds.end(); it++)
                {
                    if(*it == functions[i].get_to())
                    {
                        return;
                    }
                }
                e_closure(functions[i].get_to(), letter, 1, conds);
            }
            // нашли нужную функцию с эпсилон переходом (флаг говорит нам что мы ищём не эпсилон переход)
            if (functions[i].get_from() == state && functions[i].get_symbol() == letter && flag == 1)
            {
                conds.insert(functions[i].get_to()); // добовляем в список элемент from так как из него в самого себя идёт эпсилон переход
                e_closure(functions[i].get_to(), letter, 2, conds);
            }
            // нашли нужную функцию с эпсилон переходом (флаг говорит нам что мы ищём эпсилон переход второй раз)
            if (functions[i].get_from() == state && functions[i].get_symbol() == '^' && flag == 2)
            {
                // сверяем содержит ли уже пройденный путь данное состояние, если да то рекурсивно откатываемся на шаг назад
                for(auto it = conds.begin(); it != conds.end(); it++)
                {
                    if(*it == functions[i].get_to())
                    {
                        return;
                    }
                }
                conds.insert(functions[i].get_to()); // каждое состояние добовляем в список
                e_closure(functions[i].get_to(), letter, 2, conds);
            }
        }
        flag = 1;
    }

    void find_e_closure(int state, set<int>& closure)
    {
        // у нас есть вершина и для неё мы ищем в какие состояния мы можем попасть через эпсилон переходы
        for(int i = 0; i < functions.size(); i++)
        {
            // нашли нужную функцию с эпсилон переходом (флаг говорит нам что мы ищём эпсилон переход второй раз)
            if (functions[i].get_from() == state && functions[i].get_symbol() == '^')
            {
                // сверяем содержит ли уже пройденный путь данное состояние, если да то рекурсивно откатываемся на шаг назад
                for(auto it = closure.begin(); it != closure.end(); it++)
                {
                    if(*it == functions[i].get_to())
                    {
                        return;
                    }
                }
                closure.insert(functions[i].get_to()); // если путь не содержит данного состояния, то добовляем
                find_e_closure(functions[i].get_to(), closure); // рекурсивно двигаемся дальше
            }
        }
    }

    FA eliminate_e(string letters)
    {
        set<int> new_final_states;
        set<int> closure;
        vector<set<int>> closures;
        new_final_states.insert(final_state);
        FA new_nfa{}; // инициализируем новый автомат, который будет детерминированным
        new_nfa.start_state = start_state;

        // проходимcя по всем состояниям и ищем нужные функции перехода (состояния)
        for(int k = 0; k < states.size(); k++)
        {
            // ищем множества состояний to для каждого состояния from из подмножества temp1
            for(int i = 0; i < letters.size(); i++)
            {
                set<int> subset{}; // массив, хрянящий множества состояний перехода
                this->e_closure(states[k], letters[i], 1, subset); // нашли подмножество
                // if (subset.empty()) continue; // если набор состояний пуст то нет смысла делать всё что ниже
                // проверка, а не было ли такого подмножества раньше? если не было - добавляем в очередь

                for(auto it = subset.begin(); it != subset.end(); it++)
                {
                    new_nfa.insert(k, *it, letters[i]); // добавили функции перехода
                }
            }
            this->find_e_closure(states[k], closure);
            closures.push_back(closure);

            // будет ли конечные состояния?
            // если из состояния n можно добраться в конечное состояние эпислон нка, то n - конечное состояние в просто нка)
            for(auto it = closure.begin(); it != closure.end(); it++)
            {
                if(*it == final_state)
                {
                    new_nfa.final_states.push_back(states[k]);
                }
            }
            closure.clear();
        }
        new_nfa.final_states.push_back(final_state);

        return new_nfa;
    }

    // находим алфавит нашего автомата в виде строки (с ней проще работать) - WORK
    string find_alphabet()
    {
        // (удобно использовать set, т.к он хранит только уникальные элементы)
        set<char> letters;
        for(auto & function : functions)
        {
            letters.insert(function.get_symbol()); // добавляем все буквы из переходов
        }
        // но потом удобнее работать со строкой, поэтому перемещаем все элементы в строку
        string alphabet;
        for(char letter : letters)
        {
            if(letter != '^')
            alphabet.push_back(letter);
        }
        return alphabet;
    }

    // Конечный автомат называется детерминированным, если в нем нет
    // команд, содержащих пустую цепочку (нет дуг с меткой ^),
    // и из любого состояния по любому входному символу возможен переход в точности в одно состояние
    // т.е невозможны след две функции перехода одновременно
    // q(1, a) -> 2 - переход из состояния 1 в состояние 2 по символу а
    // q(1, a) -> 3 - переход из состояния 1 в состояние 3 по символу a
    // WORK
    bool is_deterministic(const string& letters)
    {
        // из каждого состояния идёт одинаковое количество переходов, значит общее количество переходов должно
        // нацело делиться на количество букв
        if(functions.size() % letters.size() != 0)
        {
            return false;
        }

        // подразумевается, что состояния обозначены натуральными числами и без разрывов (т.е если существуют состояния
        // 1 и 3, то обязательно существует и состояние 2)
        // если среди этих функций нет повторений, значит автомат детерминирован,
        set<int> check_uniq;
        for(int i = 0; i < states.size(); i++)
        {
            for(int j = 0; j < functions.size(); j++)
            {
                if(states[i] == functions[j].get_from())
                {
                    check_uniq.insert(functions[j].get_symbol());
                }
            }
            if(check_uniq.size() != letters.size()) return false;
            check_uniq.clear();
        }
        return true;
    }

    void print_fa()
    {
        transition new_trans;
        cout << "\n" << "Final FA: " << endl;
        for(int i = 0; i < functions.size(); i++)
        {
            new_trans = functions.at(i);
            cout << "q" << new_trans.get_from() << " --> q" << new_trans.get_to() << " : Symbol - " << new_trans.get_symbol() << endl;
        }
        cout << endl << "The final states:" << endl;
        if(final_states.empty())
        {
            cout << "q" << final_state << endl;
        }
        else
        {
            for(int i = 0; i < final_states.size(); i++)
            {
                cout << "q" << final_states[i] << endl;
            }
        }
        cout << "Start state: q" << start_state << endl;
    }

    // массив вершин автомата, количество состояний автомата, номер начального состояния, строка, индекс строки, тру\фолс,
    // массив конечный состояний - WORK
    bool check(const string& stroka)
    {
        vector<unordered_map<char, int>> trans; // массив из таблиц на буквы и числы
        trans.resize(states.size());
        unordered_set<int> finish_states;
        for(int i = 0; i < final_states.size(); i++)
        {
            finish_states.insert(final_states[i]);
        }
        for(int i = 0; i < functions.size(); i++)
        {
            trans[functions[i].get_from()].insert({functions[i].get_symbol(), functions[i].get_to()});
        }

        // основная часть
        int i = start_state;
        for (auto s : stroka) {
            auto it = trans[i].find(s); // находим итератор указывающий на переход с нужной нам буквой
            // если не равен end(), значит    иначе если ничего не нашёл find, значит буквы нужной нет, следовательно и слова нет
            if (it != trans[i].end())
            {

                i = it->second; // индекс "из" меняем на иддекс "в"
            }
            else return false;
        }
        // если строка закончилась и последнее взятое состояние финальное, то возвращаем тру иначе фолс
        if (finish_states.find(i) != finish_states.end()) return true;
        else return false;
    }

};

// проверим все ли функции имеют переходы по всем буквам и только по ним

int main()
{
    int states_number; // количество состояний автомата
    int start_state; // номер начального состояния
    int final_states_number; // количество конечных состояний
    int functions_number; // количество функций переходов
    int string_number; // количество строк, распознаваемость конечным автоматом которых необходимо проверить

    cin >> states_number >> start_state >> final_states_number;

    vector<int> final_states;
    for (int i = 0; i < final_states_number; i++) // ввод конечных сосотояний
    {
        int final_cond;
        cin >> final_cond;
        final_states.push_back(final_cond);
    }

    cin >> functions_number;

    set<int> verteces;
    vector<transition> functions;
    for (int i = 0; i < functions_number; i++) // задание функций переходов
    {
        int from, to; // номера состояний автомата
        char value; // символ, соответствующий значению функции перехода
        cin >> from >> to >> value; // из какого состояний - в какое состояние - значение перехода
        transition trans{from, to, value};
        verteces.insert(from);
        verteces.insert(to);
        functions.push_back(trans);
    }

    FA fa(final_states, functions, start_state, states_number); // init fa
    // состояния тоже будут хранится отднльно на случай если они пронумерованы не последовательно, а в
    // хаотичном порядке (скажем например что состояния не (0, 1, 2, 3), а (14, 1, 3, 8))
    for(int vertece : verteces)
    {
        fa.add_state(vertece);
    }

    string alphabet; // алфавит значений функций перехода
    alphabet = fa.find_alphabet(); // в строке будут хранитЬся все уникальные буквы, кроме эпсилон символа

    FA new_fa(fa);
//    new_fa.print_fa();

    // check: is it deterministic or not? if not then determinate it
    if(!fa.is_deterministic(alphabet))
    {
        // НКА без эпсилон переходов (сначало нужно элиминировать их отдельной функцией)
        new_fa = fa.NFA_to_DFA(alphabet);
//        new_fa.print_fa();
    }

    cin >> string_number;
    auto strings = new string[string_number];
    for (int i = 0; i < string_number; i++) // наконец, следуют строки, которые и необхожимо проверить
    {
        cin >> strings[i];
        // АВТОМАТ, строка
        if (new_fa.check(strings[i])) cout << "YES" << endl;
        else cout << "NO" << endl;
    }
    delete[] strings;

    /***********************************************************
                      START REGEXP REPRESENTATION
   Всё регулярное выражение должно быть в скобочках
     . - знак конкатенации
     | - знак объдинения
     * - знак Клини
     () - служебные знаки
     ^ - обозначает эпсилон переход
     Элементарные: (a*), (a.b), (a|b)
     Expressions Example 1: (a.(b|c))
     Expressions Example 2: ((a.b.c.d.e)*)
     Expressions Example 3: (v*.m*.g*)
     Expressions Example 4: (a*|b*|c*)
     Expressions Example 5: ((a*|b*).c*)
     Expressions Example 6: ((a*.c*).(a.c))
     Expressions Example 7: ((a.b.c)|(a.b.d))
     ((a|b)*.((a.a)|b).a*)

     Проверить правильность автоматов можно здесь:
     https://cyberzhg.github.io/toolbox/nfa2dfa
   **************************************************************/

    string regexp;
    regexp = "((a.b.c)|(a.b.d))";
    FA new_e_nfa{};
    FA new_nfa{};
    FA new_dfa{};
    cout << "do work" << endl;
    new_e_nfa = new_nfa.regexp_to_nfa(regexp);
    cout << endl <<"E_NFA";
    new_e_nfa.print_fa();
    alphabet = new_e_nfa.find_alphabet();
    cout << endl << "Alphabet: " << alphabet << endl;
    new_nfa = new_e_nfa.eliminate_e(alphabet);
    cout << endl << "NFA";
    new_nfa.print_fa();
    new_dfa = new_nfa.NFA_to_DFA(alphabet);
    cout << endl << "DFA";
    new_dfa.print_fa();
    string stroka;
    // проверим распознаёт ли какую-нибуть строку
    cin >> stroka;
    if(fa.check(stroka)) { cout << "YES" << endl; } else { cout << "NO"; }

    /***********************************************************
                       END REGEXP REPRESENTATION
    **************************************************************/

    return 0;
}