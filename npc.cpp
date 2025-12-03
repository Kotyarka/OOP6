#include "npc.h"
#include "Dragon.h"
#include "StrangeKnight.h"
#include "Elf.h"
#include <sstream>

NPC::NPC(NpcType t, int _x, int _y, const std::string& _name) : 
    type(t), x(_x), y(_y), name(_name) 
{
    if (name.empty()) {
        static int counter = 0;
        name = "NPC_" + std::to_string(++counter);
    }
}

NPC::NPC(NpcType t, std::istream &is) : type(t)
{
    is >> x;
    is >> y;
    std::getline(is >> std::ws, name);
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer)
{
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win)
{
    for (auto &o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance)
{
    std::lock_guard<std::mutex> lck(mtx);
    const auto [other_x, other_y] = other->position();
    return (std::pow(x - other_x, 2) + std::pow(y - other_y, 2)) <= std::pow(distance, 2);
}

bool NPC::fight(std::shared_ptr<NPC> other)
{
    bool result = can_defeat(other->get_type());
    fight_notify(other, result);
    return result;
}

bool NPC::accept(std::shared_ptr<NPC> visitor)
{
    return visitor->fight(shared_from_this());
}

NpcType NPC::get_type() const
{
    return type;
}

std::pair<int, int> NPC::position() const
{
    return {x, y};
}

void NPC::set_name(const std::string& new_name)
{
    name = new_name;
}

std::string NPC::get_name() const
{
    return name;
}

void NPC::save(std::ostream &os)
{
    os << x << std::endl;
    os << y << std::endl;
    os << name << std::endl;
}

std::ostream &operator<<(std::ostream &os, NPC &npc)
{
    os << "{Тип: " << npc.get_type_str() 
       << ", Имя: " << npc.get_name() 
       << ", x:" << npc.x << ", y:" << npc.y 
       << ", " << (npc.alive ? "жив" : "мертв") << "}";
    return os;
}

void NPC::move(int shift_x, int shift_y, int max_x, int max_y)
{
    std::lock_guard<std::mutex> lck(mtx);
    if ((x + shift_x >= 0) && (x + shift_x <= max_x))
        x += shift_x;
    if ((y + shift_y >= 0) && (y + shift_y <= max_y))
        y += shift_y;
}

bool NPC::is_alive() const
{
    return alive;
}

void NPC::must_die()
{
    std::lock_guard<std::mutex> lck(mtx);
    alive = false;
}