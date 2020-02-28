#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <iostream>
#include <set>

/**
 * Как избавиться от дублирования кода?
 */

static const int min_n = 1;

template<typename _Type>
std::ostream &operator<<(std::ostream &stream, const std::vector<_Type> elements) {
    int i = 0;

    stream << "(";
    for (const auto &element: elements) {
        if (i > 0)
            stream << ", ";
        stream << element;
        ++i;
    }
    stream << ")";
    return stream;
}

std::vector<int> random_elements(size_t min, size_t max, size_t count) {
    std::vector<int> result;
    std::set<int> already_exists;

    std::srand(std::time(nullptr));
    result.resize(count);
    for (auto &element: result) {
        do {
            element = min_n + (std::rand() % static_cast<int>(max - min + 1));
        } while (already_exists.find(element) != already_exists.end());
        already_exists.insert(element);
    }
    return result;
}

std::vector<int> random_elements_c11(size_t min, size_t max, size_t count) {
    std::vector<int> result;
    std::mt19937 random(static_cast<int>(std::time(nullptr)));
    std::uniform_int_distribution<int> generator(min, max);
    std::set<int> already_exists;

    result.resize(count);
    for (auto &element: result) {
        do {
            element = generator(random);
        } while (already_exists.find(element) != already_exists.end());
        already_exists.insert(element);
    }
    return result;
}

std::vector<int> sequence_elements(size_t _min, size_t _max, size_t count) {
    std::vector<int> result;
    int i = 0;

    result.resize(count);
    for (auto &element: result) {
        element = ++i;
    }
    return result;
}

template<typename _Element, typename _Functor>
void permutation(const std::vector<_Element> &original, _Functor functor) {
    int i = 1;

    std::vector<_Element> elements(original);
    while (!elements.empty()) {
        auto &element = *elements.begin();

        std::cout << element;
        if (functor(i, element)) {
            std::cout << "!";
        }
        std::cout << " ";
        elements.erase(elements.begin());
        ++i;
    }
}

int main(int argc, char **argv) {
    auto bead_count = 10;

    if (argc == 2) {
        bead_count = strtol(argv[1], nullptr, 10);
    }
    std::vector<int> beads = std::move(random_elements_c11(min_n, bead_count, bead_count));
    std::cout << beads << std::endl;
    permutation(beads, [](int i, int &element) { return i == element; });
    return EXIT_SUCCESS;
}