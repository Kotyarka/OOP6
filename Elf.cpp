#include "elf.h"
#include "dragon.h"
#include "knight.h"
#include <iostream>

Elf::Elf(int x, int y, const std::string& name) : NPC(ElfType, x, y, name) {}
Elf::Elf(std::istream &is) : NPC(ElfType, is) {}

bool Elf::accept(std::shared_ptr<NPC> visitor){
    return visitor->fight(std::shared_ptr<Elf>(this,[](Elf*){}));
}

void Elf::print()
{
    std::cout << *this << std::endl;
}

bool Elf::fight(std::shared_ptr<Dragon> other) 
{
    // Эльф vs Дракон: ПРОИГРЫВАЕТ ДРАКОНУ
    fight_notify(other, false);
    return false;  // Проигрывает дракону
}

bool Elf::fight(std::shared_ptr<Knight> other) 
{
    // Эльф vs Странствующий рыцарь: ЭЛЬФ УБИВАЕТ РЫЦАРЯ
    fight_notify(other, true);
    return true;  // Всегда побеждает рыцаря
}

bool Elf::fight(std::shared_ptr<Elf> other) 
{
    // Эльф vs Эльф: НИКТО НЕ ПОБЕЖДАЕТ
    fight_notify(other, false);
    return false;  // Не могут убить друг друга
}

void Elf::save(std::ostream &os) 
{
    os << ElfType << std::endl;
    NPC::save(os);
}

std::ostream &operator<<(std::ostream &os, Elf &elf)
{
    os << "Эльф: " << *static_cast<NPC *>(&elf);
    return os;
}