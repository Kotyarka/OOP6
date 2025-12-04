#include "Elf.h"
#include <iostream>

Elf::Elf(int x, int y, const std::string& name) : NPC(ElfType, x, y, name) {}
Elf::Elf(std::istream &is) : NPC(ElfType, is) {}

bool Elf::can_defeat(NpcType defender_type) const
{
    return defender_type == KnightType;
}

void Elf::print()
{
    std::cout << *this << std::endl;
}

void Elf::save(std::ostream &os) 
{
    os << ElfType << std::endl;
    NPC::save(os);
}

std::string Elf::get_type_str() const
{
    return "Elf";
}

std::ostream &operator<<(std::ostream &os, Elf &elf)
{
    os << "Elf: " << *static_cast<NPC *>(&elf);
    return os;
}