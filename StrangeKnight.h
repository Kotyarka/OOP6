#pragma once
#include "npc.h"

class Knight : public NPC
{
public:
    Knight(int x, int y, const std::string& name = "");
    Knight(std::istream &is);
    
    void print() override;
    void save(std::ostream &os) override;
    std::string get_type_str() const override;
    
    friend std::ostream &operator<<(std::ostream &os, Knight &knight);

protected:
    bool can_defeat(NpcType defender_type) const override;
};