#include "Dragon.h"
#include <iostream>

Dragon::Dragon(int x, int y, const std::string& name) : NPC(DragonType, x, y, name) {}
Dragon::Dragon(std::istream &is) : NPC(DragonType, is) {}

bool Dragon::can_defeat(NpcType defender_type) const
{
    return true;
}

void Dragon::print()
{
    std::cout << *this << std::endl;
}

void Dragon::save(std::ostream &os) 
{
    os << DragonType << std::endl;
    NPC::save(os);
}

std::string Dragon::get_type_str() const
{
    return "Elf";
}

std::ostream &operator<<(std::ostream &os, Dragon &dragon)
{
    os << "Elf: " << *static_cast<NPC *>(&dragon);
    return os;
}