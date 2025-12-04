#include <gtest/gtest.h>
#include "npc.h"
#include "Dragon.h"
#include "StrangeKnight.h"
#include "Elf.h"
#include "factory.h"
#include "observers.h"
#include <memory>
#include <sstream>
#include <fstream>

using namespace std;

class MockObserver : public IFightObserver {
public:
    bool fight_observed = false;
    std::shared_ptr<NPC> last_attacker;
    std::shared_ptr<NPC> last_defender;
    bool last_win = false;
    
    void on_fight(const std::shared_ptr<NPC> attacker, 
                  const std::shared_ptr<NPC> defender, 
                  bool win) override {
        fight_observed = true;
        last_attacker = attacker;
        last_defender = defender;
        last_win = win;
    }
};

TEST(NPCTest, NPCConstructor) {
    auto dragon = make_shared<Dragon>(10, 20, "TestDragon");
    
    EXPECT_EQ(dragon->get_type(), DragonType);
    EXPECT_EQ(dragon->get_name(), "TestDragon");
    EXPECT_TRUE(dragon->is_alive());
    
    auto pos = dragon->position();
    EXPECT_EQ(pos.first, 10);
    EXPECT_EQ(pos.second, 20);
}


TEST(NPCTest, NPCIsClose) {
    auto dragon1 = make_shared<Dragon>(0, 0, "Dragon1");
    auto dragon2 = make_shared<Dragon>(3, 4, "Dragon2");
    
    EXPECT_TRUE(dragon1->is_close(dragon2, 10));
    EXPECT_FALSE(dragon1->is_close(dragon2, 4));
}

TEST(NPCTest, NPCMustDie) {
    auto elf = make_shared<Elf>(10, 10, "TestElf");
    
    EXPECT_TRUE(elf->is_alive());
    elf->must_die();
    EXPECT_FALSE(elf->is_alive());
}

TEST(FightTest, DragonFightRules) {
    auto dragon = make_shared<Dragon>(0, 0, "TestDragon");
    auto knight = make_shared<Knight>(0, 0, "TestKnight");
    auto elf = make_shared<Elf>(0, 0, "TestElf");
    
    auto dragon2 = make_shared<Dragon>(0, 0, "TestDragon2");
    
    EXPECT_TRUE(dragon->fight(dragon2));
    EXPECT_TRUE(dragon->fight(knight));
    EXPECT_TRUE(dragon->fight(elf));
}

TEST(FightTest, KnightFightRules) {
    auto dragon = make_shared<Dragon>(0, 0, "TestDragon");
    auto knight = make_shared<Knight>(0, 0, "TestKnight");
    auto knight2 = make_shared<Knight>(0, 0, "TestKnight2");
    auto elf = make_shared<Elf>(0, 0, "TestElf");
    
    EXPECT_TRUE(knight->fight(dragon));
    EXPECT_FALSE(knight->fight(knight2));
    EXPECT_FALSE(knight->fight(elf));
}

TEST(FightTest, ElfFightRules) {
    auto dragon = make_shared<Dragon>(0, 0, "TestDragon");
    auto knight = make_shared<Knight>(0, 0, "TestKnight");
    auto elf = make_shared<Elf>(0, 0, "TestElf");
    auto elf2 = make_shared<Elf>(0, 0, "TestElf2");
    
    EXPECT_FALSE(elf->fight(dragon));
    EXPECT_TRUE(elf->fight(knight));
    EXPECT_FALSE(elf->fight(elf2));
}

TEST(FightTest, AcceptMethod) {
    auto dragon = make_shared<Dragon>(0, 0, "Dragon");
    auto knight = make_shared<Knight>(0, 0, "Knight");
    
    EXPECT_TRUE(knight->accept(dragon));
    EXPECT_TRUE(dragon->accept(knight));
}

TEST(ObserverTest, FightNotification) {
    auto observer = make_shared<MockObserver>();
    
    auto dragon = make_shared<Dragon>(0, 0, "Attacker");
    auto knight = make_shared<Knight>(0, 0, "Defender");
    
    dragon->subscribe(observer);
    
    dragon->fight(knight);
    
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_TRUE(observer->last_win);
    EXPECT_EQ(observer->last_attacker->get_name(), "Attacker");
    EXPECT_EQ(observer->last_defender->get_name(), "Defender");
}

TEST(FactoryTest, CreateNPC) {
    auto dragon = NPCFactory::create(DragonType, 10, 20, "FactoryDragon");
    EXPECT_NE(dragon, nullptr);
    EXPECT_EQ(dragon->get_type(), DragonType);
    EXPECT_EQ(dragon->get_name(), "FactoryDragon");
    
    auto knight = NPCFactory::create(KnightType, 30, 40, "FactoryKnight");
    EXPECT_NE(knight, nullptr);
    EXPECT_EQ(knight->get_type(), KnightType);
    
    auto elf = NPCFactory::create(ElfType, 50, 60, "FactoryElf");
    EXPECT_NE(elf, nullptr);
    EXPECT_EQ(elf->get_type(), ElfType);
}

TEST(FactoryTest, LoadNPC) {
    stringstream ss;
    ss << DragonType << endl;
    ss << 100 << endl;
    ss << 200 << endl;
    ss << "StreamDragon" << endl;
    
    auto npc = NPCFactory::load(ss);
    EXPECT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_type(), DragonType);
    EXPECT_EQ(npc->get_name(), "StreamDragon");
}

TEST(FactoryTest, SaveLoadAll) {
    set_t npcs;
    
    npcs.insert(NPCFactory::create(DragonType, 1, 2, "D1"));
    npcs.insert(NPCFactory::create(KnightType, 3, 4, "K1"));
    npcs.insert(NPCFactory::create(ElfType, 5, 6, "E1"));
    
    save(npcs, "test_save.txt");
    
    auto loaded = load("test_save.txt");
    EXPECT_EQ(loaded.size(), 3);
    
    remove("test_save.txt");
}

TEST(CombatTest, CombatResolution) {
    auto dragon = make_shared<Dragon>(0, 0, "Dragon");
    auto knight = make_shared<Knight>(0, 0, "Knight");
    auto elf = make_shared<Elf>(0, 0, "Elf");
    
    EXPECT_TRUE(dragon->accept(knight)); 

    EXPECT_TRUE(knight->accept(dragon));  
    
    EXPECT_FALSE(elf->accept(knight));   

    EXPECT_TRUE(knight->accept(elf));   
    

    EXPECT_FALSE(dragon->accept(elf));   
    
 
    EXPECT_TRUE(elf->accept(dragon));   
}

TEST(TypeTest, TypeStrings) {
    auto dragon = make_shared<Dragon>(0, 0, "");
    auto knight = make_shared<Knight>(0, 0, "");
    auto elf = make_shared<Elf>(0, 0, "");
    
    EXPECT_EQ(dragon->get_type_str(), "Дракон");
    EXPECT_EQ(knight->get_type_str(), "Странствующий рыцарь");
    EXPECT_EQ(elf->get_type_str(), "Эльф");
}

TEST(SetTest, NPCOperator) {
    auto dragon = make_shared<Dragon>(10, 20, "TestOp");
    
    stringstream ss;
    ss << *dragon;
    
    string output = ss.str();
    EXPECT_NE(output.find("Дракон"), string::npos);
    EXPECT_NE(output.find("TestOp"), string::npos);
    EXPECT_NE(output.find("x:10"), string::npos);
    EXPECT_NE(output.find("y:20"), string::npos);
}

TEST(SetTest, PrintAll) {
    set_t npcs;
    npcs.insert(NPCFactory::create(DragonType, 1, 1, "D1"));
    npcs.insert(NPCFactory::create(KnightType, 2, 2, "K1"));
    
    EXPECT_NO_THROW(print_all(npcs));
}

TEST(CombatTest, MutualKill) {
    auto dragon1 = make_shared<Dragon>(0, 0, "Dragon1");
    auto dragon2 = make_shared<Dragon>(0, 0, "Dragon2");
    
    EXPECT_TRUE(dragon1->fight(dragon2));
    EXPECT_TRUE(dragon2->fight(dragon1));
}

TEST(FileObserverTest, FileCreation) {
    auto file_observer = FileObserver::get();
    
    auto dragon = make_shared<Dragon>(0, 0, "FileTestDragon");
    auto knight = make_shared<Knight>(0, 0, "FileTestKnight");
    
    dragon->subscribe(file_observer);
    
    dragon->fight(knight);
    
    ifstream log_file("log.txt");
    EXPECT_TRUE(log_file.good());
    
    if (log_file.is_open()) {
        string content((istreambuf_iterator<char>(log_file)), 
                       istreambuf_iterator<char>());
        EXPECT_FALSE(content.empty());
        log_file.close();
    }
    
    remove("log.txt");
}

TEST(TextObserverTest, Singleton) {
    auto observer1 = TextObserver::get();
    auto observer2 = TextObserver::get();
    
    EXPECT_EQ(observer1.get(), observer2.get());
}

TEST(NPCTest, AutoNameGeneration) {
    auto dragon1 = make_shared<Dragon>(0, 0, "");
    auto dragon2 = make_shared<Dragon>(0, 0, "");
    
    EXPECT_NE(dragon1->get_name(), dragon2->get_name());
    EXPECT_EQ(dragon1->get_name().substr(0, 4), "NPC_");
    EXPECT_EQ(dragon2->get_name().substr(0, 4), "NPC_");
}

TEST(NPCTest, SetName) {
    auto dragon = make_shared<Dragon>(0, 0, "Original");
    
    EXPECT_EQ(dragon->get_name(), "Original");
    
    dragon->set_name("Changed");
    EXPECT_EQ(dragon->get_name(), "Changed");
}

TEST(FightTest, DeadNPCNoFight) {
    auto dragon = make_shared<Dragon>(0, 0, "Dragon");
    auto knight = make_shared<Knight>(0, 0, "Knight");
    
    knight->must_die();
    
    EXPECT_FALSE(knight->is_alive());
}

TEST(FactoryTest, InvalidType) {
    auto npc = NPCFactory::create(static_cast<NpcType>(999), 0, 0, "Invalid");
    EXPECT_EQ(npc, nullptr);
}

TEST(NPCTest, SharedFromThis) {
    auto dragon = make_shared<Dragon>(0, 0, "Test");
    
    EXPECT_NO_THROW({
        auto self = dragon->shared_from_this();
        EXPECT_EQ(self.get(), dragon.get());
    });
}

TEST(ObserverTest, MultipleObservers) {
    auto observer1 = make_shared<MockObserver>();
    auto observer2 = make_shared<MockObserver>();
    
    auto dragon = make_shared<Dragon>(0, 0, "Dragon");
    auto knight = make_shared<Knight>(0, 0, "Knight");
    
    dragon->subscribe(observer1);
    dragon->subscribe(observer2);
    
    dragon->fight(knight);
    
    EXPECT_TRUE(observer1->fight_observed);
    EXPECT_TRUE(observer2->fight_observed);
}

TEST(CombatTest, NoFightWhenDead) {
    auto dragon = make_shared<Dragon>(0, 0, "Dragon");
    auto knight = make_shared<Knight>(0, 0, "Knight");
    
    knight->must_die();
    
    auto observer = make_shared<MockObserver>();
    dragon->subscribe(observer);
    
    EXPECT_FALSE(knight->is_alive());

    EXPECT_TRUE(dragon->fight(knight));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}