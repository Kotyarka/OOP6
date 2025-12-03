#include "factory.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <optional>
#include <array>
#include <limits>

using namespace std::chrono_literals;
std::mutex print_mutex;

struct print : std::stringstream
{
    ~print()
    {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lck(print_mutex);
        std::cout << this->str();
        std::cout.flush();
    }
};

struct FightEvent
{
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class FightManager
{
private:
    std::queue<FightEvent> events;
    FightManager() {}
    std::mutex mtx;
    
public:
    static FightManager &get()
    {
        static FightManager instance;
        return instance;
    }

    void add_event(FightEvent &&event)
    {
        std::lock_guard<std::mutex> lck(mtx);
        events.push(event);
    }

    void clear_events()
    {
        std::lock_guard<std::mutex> lck(mtx);
        while (!events.empty()) {
            events.pop();
        }
    }

    void operator()()
    {
        while (true)
        {
            std::optional<FightEvent> event;
            if (!events.empty())
            {
                std::lock_guard<std::mutex> lck(mtx);
                event = events.front();
                events.pop();
            }

            if (event)
                if (event->attacker->is_alive())
                    if (event->defender->is_alive())
                    {
                        bool attacker_wins = event->defender->accept(event->attacker);
                        bool defender_wins = event->attacker->accept(event->defender);
                        
                        if (attacker_wins && defender_wins)
                        {
                            event->attacker->must_die();
                            event->defender->must_die();
                        }
                        else if (attacker_wins)
                        {
                            event->defender->must_die();
                        }
                        else if (defender_wins)
                        {
                            event->attacker->must_die();
                        }
                    }
            
            std::this_thread::sleep_for(100ms);
        }
    }
};

void clear_input()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void add_npc_manual(set_t& npcs)
{
    std::cout << "\n=== ДОБАВЛЕНИЕ NPC ===" << std::endl;
    std::cout << "1. Дракон" << std::endl;
    std::cout << "2. Странствующий рыцарь" << std::endl;
    std::cout << "3. Эльф" << std::endl;
    
    int type_choice;
    std::cin >> type_choice;
    
    if (type_choice < 1 || type_choice > 3)
    {
        clear_input();
        return;
    }
    
    std::cout << "X (0-500): ";
    int x;
    std::cin >> x;
    
    std::cout << "Y (0-500): ";
    int y;
    std::cin >> y;
    
    if (x < 0 || x > 500 || y < 0 || y > 500)
    {
        clear_input();
        return;
    }
    
    clear_input();
    std::cout << "Имя: ";
    std::string name;
    std::getline(std::cin, name);
    
    NpcType type = static_cast<NpcType>(type_choice);
    auto npc = NPCFactory::create(type, x, y, name);
    
    if (npc)
    {
        npcs.insert(npc);
        npc->print();
    }
}

void remove_npc(set_t& npcs)
{
    std::cout << "\n=== УДАЛЕНИЕ NPC ===" << std::endl;
    print_all(npcs);
    
    std::cout << "Номер: ";
    int index;
    std::cin >> index;
    
    if (index < 1 || index > npcs.size())
    {
        clear_input();
        return;
    }
    
    auto it = npcs.begin();
    std::advance(it, index - 1);
    npcs.erase(it);
    
    clear_input();
}

void start_combat_mode(set_t& npcs)
{
    std::cout << "\n=== БОЕВОЙ РЕЖИМ ===" << std::endl;
    
    int distance;
    std::cout << "Дальность боя: ";
    std::cin >> distance;
    
    if (distance <= 0)
    {
        std::cout << "Дальность должна быть положительной!" << std::endl;
        clear_input();
        return;
    }
    
    clear_input();
    
    const int MAX_X = 500;
    const int MAX_Y = 500;
    const int DISTANCE = distance;
    
    FightManager::get().clear_events();
    
    std::cout << "Боевой режим запущен! Нажмите Enter для остановки..." << std::endl;
    
    std::thread fight_thread(std::ref(FightManager::get()));
    
    bool combat_running = true;
    
    std::thread move_thread([&npcs, MAX_X, MAX_Y, DISTANCE, &combat_running]()
    {
        while (combat_running)
        {
            for (const std::shared_ptr<NPC> & npc : npcs)
                if(npc->is_alive())
                    npc->move(std::rand() % 40 - 20, 
                              std::rand() % 40 - 20, MAX_X, MAX_Y);

            for (const std::shared_ptr<NPC> & npc : npcs)
                for (const std::shared_ptr<NPC> & other : npcs) 
                    if ((other != npc) && 
                        (npc->is_alive()) && 
                        (other->is_alive()) && 
                        (npc->is_close(other, DISTANCE)))
                        FightManager::get().add_event({npc, other});
            
            std::this_thread::sleep_for(10ms);
        }
    });
    
    const int grid = 20;
    const int step_x = MAX_X / grid;
    const int step_y = MAX_Y / grid;
    
    std::thread input_thread([&combat_running]() {
        std::cin.get();
        combat_running = false;
    });
    
    while (combat_running)
    {
        std::array<char, grid * grid> fields{0};
        
        for (const std::shared_ptr<NPC> &npc : npcs)
        {
            const auto [x, y] = npc->position();
            int i = x / step_x;
            int j = y / step_y;
            
            if (i >= 0 && i < grid && j >= 0 && j < grid)
            {
                if (npc->is_alive())
                {
                    switch (npc->get_type())
                    {
                    case DragonType:
                        fields[i + grid * j] = 'D';
                        break;
                    case KnightType:
                        fields[i + grid * j] = 'K';
                        break;
                    case ElfType:
                        fields[i + grid * j] = 'E';
                        break;
                    default:
                        break;
                    }
                }
                else
                {
                    fields[i + grid * j] = '.';
                }
            }
        }
        
        std::cout << "\033[2J\033[1;1H";
        
        std::cout << "=== БОЕВОЙ РЕЖИМ ===" << std::endl;
        std::cout << "D - Дракон, K - Рыцарь, E - Эльф, . - мертвый" << std::endl;
        std::cout << "Нажмите Enter для остановки" << std::endl << std::endl;
        
        for (int j = 0; j < grid; ++j)
        {
            for (int i = 0; i < grid; ++i)
            {
                char c = fields[i + j * grid];
                if (c != 0)
                    std::cout << "[" << c << "]";
                else
                    std::cout << "[ ]";
            }
            std::cout << std::endl;
        }
        
        int alive_count = 0;
        int dragon_count = 0;
        int knight_count = 0;
        int elf_count = 0;
        
        for (const auto& npc : npcs)
        {
            if (npc->is_alive())
            {
                alive_count++;
                switch (npc->get_type())
                {
                case DragonType: dragon_count++; break;
                case KnightType: knight_count++; break;
                case ElfType: elf_count++; break;
                }
            }
        }
        
        std::cout << std::endl;
        std::cout << "Статистика:" << std::endl;
        std::cout << "Живых: " << alive_count << " (Драконов: " << dragon_count 
                  << ", Рыцарей: " << knight_count << ", Эльфов: " << elf_count << ")" << std::endl;
        std::cout << "Мертвых: " << (npcs.size() - alive_count) << std::endl;
        
        std::this_thread::sleep_for(500ms);
    }
    
    input_thread.join();
    
    if (move_thread.joinable())
        move_thread.join();
    
    if (fight_thread.joinable())
        fight_thread.detach();
    
    std::cout << "\nБоевой режим остановлен!" << std::endl;
    
    set_t alive_npcs;
    for (auto& npc : npcs)
    {
        if (npc->is_alive())
            alive_npcs.insert(npc);
    }
    npcs = std::move(alive_npcs);
    
    std::cout << "Осталось живых NPC: " << npcs.size() << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.get();
}

void editor_mode(set_t& npcs)
{
    bool running = true;
    
    while (running)
    {
        std::cout << "\n=== РЕДАКТОР NPC ===" << std::endl;
        std::cout << "1. Добавить NPC" << std::endl;
        std::cout << "2. Удалить NPC" << std::endl;
        std::cout << "3. Показать всех NPC" << std::endl;
        std::cout << "4. Сохранить в файл" << std::endl;
        std::cout << "5. Загрузить из файла" << std::endl;
        std::cout << "6. Запустить боевой режим" << std::endl;
        std::cout << "7. Сгенерировать случайных NPC" << std::endl;
        std::cout << "8. Быстрый бой" << std::endl;
        std::cout << "9. Выйти" << std::endl;
        std::cout << "Выбор: ";
        
        int choice;
        std::cin >> choice;
        clear_input();
        
        switch (choice)
        {
        case 1:
            add_npc_manual(npcs);
            break;
            
        case 2:
            remove_npc(npcs);
            break;
            
        case 3:
            print_all(npcs);
            break;
            
        case 4:
            {
                std::string filename;
                std::cout << "Имя файла: ";
                std::getline(std::cin, filename);
                save(npcs, filename);
            }
            break;
            
        case 5:
            {
                std::string filename;
                std::cout << "Имя файла: ";
                std::getline(std::cin, filename);
                npcs = load(filename);
            }
            break;
            
        case 6:
            start_combat_mode(npcs);
            break;
            
        case 7:
            {
                std::cout << "Сколько NPC сгенерировать? ";
                int count;
                std::cin >> count;
                clear_input();
                
                for (int i = 0; i < count && i < 100; ++i)
                {
                    NpcType type = static_cast<NpcType>((std::rand() % 3) + 1);
                    int x = std::rand() % 501;
                    int y = std::rand() % 501;
                    auto npc = NPCFactory::create(type, x, y);
                    if (npc) npcs.insert(npc);
                }
                std::cout << "Сгенерировано " << std::min(count, 100) << " NPC" << std::endl;
            }
            break;
            
        case 8:
            {
                std::cout << "Дальность быстрого боя: ";
                int distance;
                std::cin >> distance;
                clear_input();
                
                if (distance > 0)
                {
                    set_t dead_list;
                    
                    for (const auto &attacker : npcs)
                        for (const auto &defender : npcs)
                            if ((attacker != defender) &&
                                attacker->is_alive() &&
                                defender->is_alive() &&
                                attacker->is_close(defender, distance))
                            {
                                bool attacker_wins = defender->accept(attacker);
                                bool defender_wins = attacker->accept(defender);
                                
                                if (attacker_wins && defender_wins)
                                {
                                    attacker->must_die();
                                    defender->must_die();
                                    dead_list.insert(attacker);
                                    dead_list.insert(defender);
                                }
                                else if (attacker_wins)
                                {
                                    defender->must_die();
                                    dead_list.insert(defender);
                                }
                                else if (defender_wins)
                                {
                                    attacker->must_die();
                                    dead_list.insert(attacker);
                                }
                            }
                    
                    set_t alive_npcs;
                    for (auto& npc : npcs)
                    {
                        if (npc->is_alive())
                            alive_npcs.insert(npc);
                    }
                    npcs = std::move(alive_npcs);
                    
                    std::cout << "Бой завершен. Убито: " << dead_list.size() 
                              << ", осталось: " << npcs.size() << std::endl;
                }
            }
            break;
            
        case 9:
            running = false;
            break;
            
        default:
            std::cout << "Неверный выбор!" << std::endl;
            break;
        }
    }
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    set_t npcs;
    
    std::cout << "=== СИМУЛЯТОР NPC ===" << std::endl;
    std::cout << "Правила боев:" << std::endl;
    std::cout << "- Дракон убивает всех (включая других драконов)" << std::endl;
    std::cout << "- Странствующий рыцарь убивает дракона" << std::endl;
    std::cout << "- Эльф убивает странствующего рыцаря" << std::endl;
    std::cout << "- Взаимные убийства возможны" << std::endl;
    std::cout << "====================" << std::endl;
    
    std::cout << "Генерация начальных NPC..." << std::endl;
    for (size_t i = 0; i < 10; ++i)
    {
        NpcType type = static_cast<NpcType>((std::rand() % 3) + 1);
        int x = std::rand() % 501;
        int y = std::rand() % 501;
        auto npc = NPCFactory::create(type, x, y);
        if (npc) npcs.insert(npc);
    }
    
    editor_mode(npcs);
    
    return 0;
}