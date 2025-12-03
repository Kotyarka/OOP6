#include "dragon.h"
#include "knight.h"
#include "elf.h"
#include <iostream>

Dragon::Dragon(int x, int y, const std::string& name) : NPC(DragonType, x, y, name) {}
Dragon::Dragon(std::istream &is) : NPC(DragonType, is) {}

bool Dragon::accept(std::shared_ptr<NPC> visitor){
    return visitor->fight(std::shared_ptr<Dragon>(this,[](Dragon*){}));
}

void Dragon::print()
{
    std::cout << *this << std::endl;
}

bool Dragon::fight(std::shared_ptr<Dragon> other) 
{
    // Дракон vs Дракон: ДРАКОН ПОБЕЖДАЕТ ВСЕХ, ВКЛЮЧАЯ ДРУГИХ ДРАКОНОВ
    fight_notify(other, true);
    return true;  // Всегда побеждает
}

bool Dragon::fight(std::shared_ptr<Knight> other) 
{
    // Дракон vs Странствующий рыцарь: ДРАКОН УБИВАЕТ ВСЕХ
    fight_notify(other, true);
    return true;  // Всегда побеждает
}

bool Dragon::fight(std::shared_ptr<Elf> other) 
{
    // Дракон vs Эльф: ДРАКОН УБИВАЕТ ВСЕХ
    fight_notify(other, true);
    return true;  // Всегда побеждает
}

void Dragon::save(std::ostream &os) 
{
    os << DragonType << std::endl;
    NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, Dragon &dragon)
{
    os << "Дракон: " << *static_cast<NPC *>(&dragon);
    return os;
}