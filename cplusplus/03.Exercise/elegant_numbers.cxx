#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <iostream>
#include <set>

/*
 * Оптимизировать ввод данных, спрятать его в абстрактном классе
 */

struct NumberFunction {
    typedef int _NumberType;

    virtual std::string name() const = 0;

    virtual bool has_range() const = 0;
};

struct NumberRangeFunction : public NumberFunction {
    bool has_range() const override {
        return true;
    }

    virtual void execute(std::ostream &output, const _NumberType &beginRange, const _NumberType &endRange) = 0;
};

struct NumberOneFunction : public NumberFunction {
    bool has_range() const override {
        return false;
    }

    virtual void execute(std::ostream &output, const _NumberType &endRange) = 0;
};

struct PythagoreNumber : public NumberRangeFunction {
    void execute(std::ostream &output, const _NumberType &beginRange, const _NumberType &endRange) override {
        for (_NumberType x = beginRange; x < endRange; ++x) {
            for (_NumberType y = beginRange; y < endRange; ++y) {
                for (_NumberType z = beginRange; z < endRange; ++z) {
                    if (std::pow(x, 2) + std::pow(y, 2) == std::pow(z, 2)) {
                        output << "(" << x << ", " << y << ", " << z << ")" << std::endl;
                    }
                }
            }
        }
    }

    std::string name() const override {
        return "Pythagores";
    }
};

struct FibonacciNumber : public NumberOneFunction {
    class Fibonacci {
        std::ostream &_output;
        int _i;

        explicit Fibonacci(std::ostream &output)
                : _output(output), _i(0) {}

        _NumberType next(const _NumberType &n) {
            if (_i > 0)
                _output << ", ";
            if (n == 1 || n == 2) {
                _output << "1";
                _i++;
                return 1;
            }
            const auto &v = next(n - 1) + next(n - 2);
            _output << v;
            _i++;
            return v;
        }
    };

    std::string name() const override {
        return "Fibonacci";
    }

    void execute(std::ostream &output, const _NumberType &endRange) override {
        output << "(";
        Fibonacci(output).next(endRange);
        output << ")" << std::endl;
    }
};

static std::vector<NumberFunction *> functions;

int main(int argc, char **argv) {
    std::ostream &output = std::cout;

    functions.push_back(new PythagoreNumber());
    functions.push_back(new FibonacciNumber());

    while (true) {
        auto select = 0;
        for (auto i = 0; i < functions.size(); ++i) {
            auto &func = functions.at(i);
            std::cout << i << ". " << func->name() << " number." << std::endl;
        }
        std::cout << functions.size() << ". Exit" << std::endl;
        std::cout << "Select action by number: ";
        std::cin >> select;
        if (select == functions.size())
            break;
        if (select >= 0 && select < functions.size()) {
            auto &func = functions.at(select);
            if (func->has_range()) {
                NumberFunction::_NumberType beginRange;
                NumberFunction::_NumberType endRange;

                std::cout << "Input begin range: ";
                std::cin >> beginRange;
                std::cout << "Input end range: ";
                std::cin >> endRange;
                dynamic_cast<NumberRangeFunction *>(func)->execute(output, beginRange, endRange);
            } else {
                NumberFunction::_NumberType beginRange;

                std::cout << "Input begin range: ";
                std::cin >> beginRange;
                dynamic_cast<NumberOneFunction *>(func)->execute(output, beginRange);
            }
        } else {
            std::cerr << "Illegal select." << std::endl;
            std::cerr.flush();
        }
    }
    return EXIT_SUCCESS;
}