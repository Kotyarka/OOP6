#pragma once
#include "npc.h"
#include "Dragon.h"
#include "StrangeKnight.h"
#include "Elf.h"
#include "observers.h"
#include <sstream>

class NPCFactory
{
public:
    static std::shared_ptr<NPC> create(NpcType type, int x, int y, const std::string& name = "")
    {
        std::shared_ptr<NPC> result;
        switch (type)
        {
        case DragonType:
            result = std::make_shared<Dragon>(x, y, name);
            break;
        case KnightType:
            result = std::make_shared<Knight>(x, y, name);
            break;
        case ElfType:
            result = std::make_shared<Elf>(x, y, name);
            break;
        default:
            return nullptr;
        }
        
        if (result)
        {
            result->subscribe(TextObserver::get());
            result->subscribe(FileObserver::get());
        }
        
        return result;
    }
    
    static std::shared_ptr<NPC> load(std::istream &is)
    {
        std::shared_ptr<NPC> result;
        int type{0};
        if (is >> type)
        {
            switch (type)
            {
            case DragonType:
                result = std::make_shared<Dragon>(is);
                break;
            case KnightType:
                result = std::make_shared<Knight>(is);
                break;
            case ElfType:
                result = std::make_shared<Elf>(is);
                break;
            default:
                return nullptr;
            }
        }
        
        if (result)
        {
            result->subscribe(TextObserver::get());
            result->subscribe(FileObserver::get());
        }
        
        return result;
    }
    
    static std::string get_type_name(NpcType type)
    {
        switch (type)
        {
        case DragonType: return "Дракон";
        case KnightType: return "Странствующий рыцарь";
        case ElfType: return "Эльф";
        default: return "Неизвестный";
        }
    }
};

void save(const set_t &array, const std::string &filename)
{
    std::ofstream fs(filename);
    if (!fs.is_open())
    {
        return;
    }
    
    fs << array.size() << std::endl;
    for (auto &n : array)
        n->save(fs);
    
    fs.flush();
    fs.close();
}

set_t load(const std::string &filename)
{
    set_t result;
    std::ifstream is(filename);
    if (is.good() && is.is_open())
    {
        int count;
        is >> count;
        for (int i = 0; i < count; ++i)
        {
            auto npc = NPCFactory::load(is);
            if (npc)
                result.insert(npc);
        }
        is.close();
    }
    return result;
}

void print_all(const set_t &array)
{
    std::cout << "\n=== СПИСОК NPC ===" << std::endl;
    std::cout << "Всего: " << array.size() << std::endl;
    
    int alive_count = 0;
    for (auto &n : array)
    {
        if (n->is_alive())
        {
            alive_count++;
            n->print();
        }
    }
    
    std::cout << "\nЖивых: " << alive_count << std::endl;
    std::cout << "Мертвых: " << (array.size() - alive_count) << std::endl;
    std::cout << "==================" << std::endl;
}