#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <string>
#include <random>
#include <fstream>
#include <set>
#include <math.h>
#include <shared_mutex>
#include <vector>
#include <functional>

struct NPC;
struct Dragon;
struct Knight;
struct BlackKnight;
using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType
{
    Unknown = 0,
    DragonType = 1,
    KnightType = 2,
    ElfType = 3 
};
struct IFightObserver
{
    virtual void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) = 0;
    virtual ~IFightObserver() = default;
};

class NPC
{
private:
    std::mutex mtx;

    NpcType type;
    int x{0};
    int y{0};
    bool alive{true};
    std::string name;

    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int _x, int _y, const std::string& _name);
    NPC(NpcType t, std::istream &is);
    
    virtual ~NPC() = default;

    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);
    virtual bool is_close(const std::shared_ptr<NPC> &other, size_t distance);

    virtual bool accept(std::shared_ptr<NPC> visitor) = 0;
    // visit
    virtual bool fight(std::shared_ptr<Dragon> other) = 0;
    virtual bool fight(std::shared_ptr<Knight> other) = 0;
    virtual bool fight(std::shared_ptr<BlackKnight> other) = 0;

    virtual void print() = 0;
    std::pair<int, int> position() const;
    NpcType get_type() const;
    
    void set_name(const std::string& new_name) { name = new_name; }
    std::string get_name() const { return name; }

    virtual void save(std::ostream &os);
    
    virtual std::string get_type_str() const = 0;

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);

    void move(int shift_x, int shift_y, int max_x, int max_y);

    bool is_alive() const;
    void must_die();
};