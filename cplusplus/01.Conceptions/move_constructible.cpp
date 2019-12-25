#include <cstdlib>
#include <string>
#include <iostream>
#include <utility>

//https://ru.cppreference.com/w/cpp/types/is_move_constructible

struct NoMoveConstructible {
    std::string m_text; // Член имеет нетривиальный, но не вызывающий исключений
    // конструктор переноса

    NoMoveConstructible(const NoMoveConstructible &) = delete;

    NoMoveConstructible(NoMoveConstructible &&) = delete;
};

struct MoveConstructible {
    std::string m_text;

    explicit MoveConstructible(std::string text) : m_text(std::move(text)) {
        std::cout << "конструктор MoveConstructible()" << std::endl;
    }

    MoveConstructible(MoveConstructible &&other) noexcept : m_text(std::move(other.m_text)) {
        std::cout << "конструктор MoveConstructible(MoveConstructible &&other)" << std::endl;
    }

    MoveConstructible(const MoveConstructible &other) noexcept : m_text(other.m_text) {
        std::cout << "конструктор MoveConstructible(const MoveConstructible &other)" << std::endl;
    }
};

struct TriviallyMoveConstructible {
    std::string m_text;

    // Исключает возможность неявного определения конструктора переноса.
    // Тем не менее, класс до сих пор перемещаемый, так как
    // его конструктор копирования может выполниться с правостороннем аргументом.
    TriviallyMoveConstructible(TriviallyMoveConstructible &&) = default; // Тривиальный и не вызывающий исключений
};

int main(int argc, char **argv) {
    MoveConstructible mc1(std::string("text1"));
    MoveConstructible mc2 = std::move(mc1);
    MoveConstructible mc3 = MoveConstructible(mc2);

    std::cout << std::boolalpha << std::string("MoveConstructible имеет семантику переноса? ")
              << std::is_move_constructible<MoveConstructible>::value << std::endl;
    std::cout << std::boolalpha << std::string("MoveConstructible имеет тривиальную семантику переноса? ")
              << std::is_trivially_move_constructible<MoveConstructible>::value
              << std::endl;
    std::cout << std::boolalpha << std::string("MoveConstructible имеет не вызывающую исключения семантику переноса? ")
              << std::is_nothrow_move_constructible<MoveConstructible>::value
              << std::endl;

    std::cout << std::boolalpha << std::string("TriviallyMoveConstructible имеет семантику переноса? ")
              << std::is_move_constructible<TriviallyMoveConstructible>::value << std::endl;
    std::cout << std::boolalpha << std::string("TriviallyMoveConstructible имеет тривиальную семантику переноса? ")
              << std::is_trivially_move_constructible<TriviallyMoveConstructible>::value
              << std::endl;
    std::cout << std::boolalpha
              << std::string("TriviallyMoveConstructible имеет не вызывающую исключения семантику переноса? ")
              << std::is_nothrow_move_constructible<TriviallyMoveConstructible>::value
              << std::endl;

    std::cout << std::boolalpha << std::string("NoMoveConstructible имеет семантику переноса? ")
              << std::is_move_constructible<NoMoveConstructible>::value << std::endl;
    std::cout << std::boolalpha << std::string("NoMoveConstructible имеет тривиальную семантику переноса? ")
              << std::is_trivially_move_constructible<NoMoveConstructible>::value
              << std::endl;
    std::cout << std::boolalpha
              << std::string("NoMoveConstructible имеет не вызывающую исключения семантику переноса? ")
              << std::is_nothrow_move_constructible<NoMoveConstructible>::value
              << std::endl;
    return EXIT_SUCCESS;
}

