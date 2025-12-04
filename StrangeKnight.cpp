#include "StrangeKnight.h"
#include <iostream>

Knight::Knight(int x, int y, const std::string& name) : NPC(KnightType, x, y, name) {}
Knight::Knight(std::istream &is) : NPC(KnightType, is) {}

bool Knight::can_defeat(NpcType defender_type) const
{
    return defender_type == DragonType;
}

void Knight::print()
{
    std::cout << *this << std::endl;
}

void Knight::save(std::ostream &os) 
{
    os << KnightType << std::endl;
    NPC::save(os);
}

std::string Knight::get_type_str() const
{
    return "SKnight";
}

std::ostream &operator<<(std::ostream &os, Knight &knight)
{
    os << "SKnight: " << *static_cast<NPC *>(&knight);
    return os;
}