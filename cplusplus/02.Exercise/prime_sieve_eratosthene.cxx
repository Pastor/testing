#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

class Eratosthene final {
    typedef std::pair<long, long> Prime;
    typedef std::vector<Prime> PrimeList;

    PrimeList _primes;
public:
    Eratosthene() {
        _primes.push_back(std::make_pair(2, 2));
        _primes.push_back(std::make_pair(3, 3));
    }

    void generate(int n) {
        while (_primes.size() < n) {
            next();
        }
    }

private:
    void next() {
        auto candidate = _primes.at(_primes.size() - 1).first + 2;
        for (PrimeList::size_type i = 1; i < _primes.size(); i++) {
            auto &p = _primes.at(i);
            while (p.second < candidate) {
                p.second += p.first;
            }
            if (p.second == candidate) {
                candidate += 2;
                i = -1;
            }
        }
        _primes.push_back(std::make_pair(candidate, candidate));
    }

    friend std::ostream &operator<<(std::ostream &stream, const Eratosthene &e);
};

std::ostream &operator<<(std::ostream &stream, const Eratosthene &e) {
    for (const auto &prime: e._primes) {
        std::cout << prime.first << std::endl;
    }
    return stream;
}

int main(int argc, char **argv) {
    Eratosthene eratosthene;

    eratosthene.generate(1000);
    std::cout << eratosthene;
    return EXIT_SUCCESS;
}
