#include "factory.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <optional>
#include <array>
#include <limits>

using namespace std::chrono_literals;

const int MAX_X = 500;
const int MAX_Y = 500;

class CombatManager
{
private:
    std::queue<std::pair<std::shared_ptr<NPC>, std::shared_ptr<NPC>>> events;
    std::mutex mtx;
    bool combat_mode = false;
    
    CombatManager() = default;

public:
    static CombatManager& get()
    {
        static CombatManager instance;
        return instance;
    }
    
    void set_combat_mode(bool mode) { combat_mode = mode; }
    bool is_combat_mode() const { return combat_mode; }
    
    void add_combat_event(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender)
    {
        if (!combat_mode) return;
        
        std::lock_guard<std::mutex> lck(mtx);
        events.push({attacker, defender});
    }
    
    void process_events(set_t& npcs)
    {
        std::lock_guard<std::mutex> lck(mtx);
        
        while (!events.empty())
        {
            auto [attacker, defender] = events.front();
            events.pop();
            
            if (!attacker->is_alive() || !defender->is_alive())
                continue;
            
            bool attacker_wins = defender->accept(attacker);
            bool defender_wins = attacker->accept(defender);
            
            if (attacker_wins && defender_wins)
            {
                attacker->must_die();
                defender->must_die();
            }
            else if (attacker_wins)
            {
                defender->must_die();
            }
            else if (defender_wins)
            {
                attacker->must_die();
            }
        }
        
        set_t alive_npcs;
        for (auto& npc : npcs)
        {
            if (npc->is_alive())
                alive_npcs.insert(npc);
        }
        npcs = std::move(alive_npcs);
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
    
    std::cout << "X (0-" << MAX_X << "): ";
    int x;
    std::cin >> x;
    
    std::cout << "Y (0-" << MAX_Y << "): ";
    int y;
    std::cin >> y;
    
    if (x < 0 || x > MAX_X || y < 0 || y > MAX_Y)
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
    std::cout << "Дальность: ";
    std::cin >> distance;
    
    if (distance <= 0)
    {
        clear_input();
        return;
    }
    
    CombatManager::get().set_combat_mode(true);
    
    std::cout << "Боевой режим запущен!" << std::endl;
    clear_input();
    std::cin.get();
    
    CombatManager::get().set_combat_mode(false);
    
    CombatManager::get().process_events(npcs);
}

void collision_checker_thread(set_t& npcs, int distance)
{
    while (true)
    {
        if (!CombatManager::get().is_combat_mode())
        {
            std::this_thread::sleep_for(100ms);
            continue;
        }
        
        for (const auto& npc1 : npcs)
        {
            if (!npc1->is_alive()) continue;
            
            for (const auto& npc2 : npcs)
            {
                if (!npc2->is_alive()) continue;
                if (npc1 == npc2) continue;
                
                if (npc1->is_close(npc2, distance))
                {
                    CombatManager::get().add_combat_event(npc1, npc2);
                }
            }
        }
        
        std::this_thread::sleep_for(50ms);
    }
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    set_t npcs;
    bool running = true;
    
    std::thread collision_thread([&npcs]() {
        collision_checker_thread(npcs, 50);
    });
    
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
        std::cout << "8. Выйти" << std::endl;
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
                std::cout << "Файл: ";
                std::getline(std::cin, filename);
                save(npcs, filename);
            }
            break;
        case 5:
            {
                std::string filename;
                std::cout << "Файл: ";
                std::getline(std::cin, filename);
                npcs = load(filename);
            }
            break;
        case 6:
            start_combat_mode(npcs);
            break;
        case 7:
            {
                std::cout << "Сколько: ";
                int count;
                std::cin >> count;
                clear_input();
                
                for (int i = 0; i < count && i < 100; ++i)
                {
                    NpcType type = static_cast<NpcType>((std::rand() % 3) + 1);
                    int x = std::rand() % (MAX_X + 1);
                    int y = std::rand() % (MAX_Y + 1);
                    auto npc = NPCFactory::create(type, x, y);
                    if (npc) npcs.insert(npc);
                }
            }
            break;
        case 8:
            running = false;
            break;
        }
        
        CombatManager::get().process_events(npcs);
    }
    
    collision_thread.detach();
    
    return 0;
}