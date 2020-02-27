#include <cstdlib>
#include <string>
#include <algorithm>
#include <iostream>

struct Functor {
    bool operator()(const char &ch1, const char &ch2) {
        return ch1 < ch2;
    }
};

struct Functor_ {
    template<typename _Type>
    bool operator()(const _Type &ch1, const _Type &ch2) {
        return ch1 < ch2;
    }
};

static bool character_comp(const char &ch1, const char &ch2) {
    return ch1 < ch2;
}

int main(int argc, char **argv) {
    std::string text("0987654321dcba");

    std::cout << "Sort default." << std::endl;
    std::cout << "Before    : '" << text << "'" << std::endl;
    std::sort(text.begin(), text.end());
    std::cout << "After     : '" << text << "'" << std::endl;

    text = "0987654321dcba";
    std::cout << "Sort less." << std::endl;
    std::cout << "Before    : '" << text << "'" << std::endl;
    std::sort(text.begin(), text.end(), std::less<>());
    std::cout << "After     : '" << text << "'" << std::endl;

    text = "0987654321dcba";
    std::cout << "Sort custom." << std::endl;
    std::cout << "Before    : '" << text << "'" << std::endl;
    std::sort(text.begin(), text.end(), character_comp);
    std::cout << "After     : '" << text << "'" << std::endl;

    text = "0987654321dcba";
    std::cout << "Sort lambda." << std::endl;
    std::cout << "Before    : '" << text << "'" << std::endl;
    std::sort(text.begin(), text.end(), [](const char &ch1, const char &ch2) { return ch1 < ch2; });
    std::cout << "After     : '" << text << "'" << std::endl;

    text = "0987654321dcba";
    std::cout << "Sort functor." << std::endl;
    std::cout << "Before    : '" << text << "'" << std::endl;
    std::sort(text.begin(), text.end(), Functor());
    std::cout << "After     : '" << text << "'" << std::endl;

    text = "0987654321dcba";
    std::cout << "Sort functor template." << std::endl;
    std::cout << "Before    : '" << text << "'" << std::endl;
    std::sort(text.begin(), text.end(), Functor_());
    std::cout << "After     : '" << text << "'" << std::endl;
    return EXIT_SUCCESS;
}
