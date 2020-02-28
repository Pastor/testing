#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <iostream>
#include <set>

struct NumberFunction {
    typedef double _NumberType;

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

struct PythagoreNumber: public NumberRangeFunction {
    bool operator()() {
        return true;
    }

    void execute(std::ostream &output, const _NumberType &beginRange, const _NumberType &endRange) override {

    }
};


int main(int argc, char **argv) {
    auto p = new PythagoreNumber();
    auto n = reinterpret_cast<NumberFunction *>(p);
    auto d = reinterpret_cast<NumberRangeFunction *>(p);
    return EXIT_SUCCESS;
}