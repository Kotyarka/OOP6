#pragma once
#include "npc.h"
#include <fstream>
#include <mutex>
#include <chrono>

class TextObserver : public IFightObserver
{
private:
    std::mutex mtx;
    TextObserver() {};

public:
    static std::shared_ptr<IFightObserver> get()
    {
        static TextObserver instance;
        return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
    }

    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) override
    {
        if (win)
        {
            std::lock_guard<std::mutex> lck(mtx);
            std::cout << "\n=== KILL ===" << std::endl;
            std::cout << "Attacker: ";
            attacker->print();
            std::cout << "Defender: ";
            defender->print();
            std::cout << "----------------" << std::endl;
        }
    }
};

class FileObserver : public IFightObserver
{
private:
    std::ofstream log_file;
    std::mutex mtx;

    FileObserver() : log_file("log.txt", std::ios::app) {}

public:
    ~FileObserver()
    {
        if (log_file.is_open())
        {
            log_file.close();
        }
    }

    static std::shared_ptr<IFightObserver> get()
    {
        static FileObserver instance;
        return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
    }

    void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) override
    {
        if (win)
        {
            std::lock_guard<std::mutex> lck(mtx);
            if (log_file.is_open())
            {
                auto now = std::chrono::system_clock::now();
                std::time_t time = std::chrono::system_clock::to_time_t(now);
                
                log_file << "\n=== Kill ===" << std::endl;
                log_file << "Time: " << std::ctime(&time);
                log_file << "Atacker: " << attacker->get_type_str() 
                         << " \"" << attacker->get_name() << "\"" << std::endl;
                log_file << "Defender: " << defender->get_type_str() 
                         << " \"" << defender->get_name() << "\"" << std::endl;
                log_file << "----------------" << std::endl;
                log_file.flush();
            }
        }
    }
};