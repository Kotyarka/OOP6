#pragma once
#include "npc.h"

class Elf : public NPC
{
public:
    Elf(int x, int y, const std::string& name = "");
    Elf(std::istream &is);
    
    bool accept(std::shared_ptr<NPC> visitor) override;
    void print() override;
    
    bool fight(std::shared_ptr<Dragon> other) override;
    bool fight(std::shared_ptr<Knight> other) override;
    bool fight(std::shared_ptr<Elf> other) override;
    
    void save(std::ostream &os) override;
    std::string get_type_str() const override { return "Эльф"; }
    
    friend std::ostream &operator<<(std::ostream &os, Elf &elf);
};