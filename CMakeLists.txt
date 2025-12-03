cmake_minimum_required(VERSION 3.10)
project(NPC_Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Основное приложение
add_executable(npc_simulator
    main.cpp
    npc.cpp
    Dragon.cpp
    StrangeKnight.cpp
    Elf.cpp
)

# Тесты
add_executable(npc_tests
    tests.cpp
    npc.cpp
    Dragon.cpp
    StrangeKnight.cpp
    Elf.cpp
)

# Подключаем GoogleTest к тестам
target_link_libraries(npc_tests ${GTEST_LIBRARIES} pthread)

# Устанавливаем директории для заголовочных файлов
target_include_directories(npc_simulator PRIVATE .)
target_include_directories(npc_tests PRIVATE .)