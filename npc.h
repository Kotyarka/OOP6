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
struct Elf;
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

class NPC : public std::enable_shared_from_this<NPC>
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
    bool is_close(const std::shared_ptr<NPC> &other, size_t distance);

    virtual bool fight(std::shared_ptr<NPC> other) final;
    virtual bool accept(std::shared_ptr<NPC> visitor);

    virtual void print() = 0;
    std::pair<int, int> position() const;
    NpcType get_type() const;
    
    void set_name(const std::string& new_name);
    std::string get_name() const;

    virtual void save(std::ostream &os);
    virtual std::string get_type_str() const = 0;

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);

    void move(int shift_x, int shift_y, int max_x, int max_y);
    bool is_alive() const;
    void must_die();

protected:
    virtual bool can_defeat(NpcType defender_type) const = 0;
};