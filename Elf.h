#pragma once
#include "npc.h"

class Elf : public NPC
{
public:
    Elf(int x, int y, const std::string& name = "");
    Elf(std::istream &is);
    
    void print() override;
    void save(std::ostream &os) override;
    std::string get_type_str() const override;
    
    friend std::ostream &operator<<(std::ostream &os, Elf &elf);

protected:
    bool can_defeat(NpcType defender_type) const override;
};