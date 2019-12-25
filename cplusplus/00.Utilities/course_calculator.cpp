#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <vector>
#include <iomanip>

constexpr wchar_t letter[] = L"аоуэыяеюи";

struct StudentInfo {
    std::wstring group;
    std::wstring number_id;
    std::wstring card_id;
    std::wstring name;

    StudentInfo(StudentInfo &&) = default;

    [[nodiscard]] std::wstring job() const {
        std::wstring result;

        for (auto &ch: name) {
            result.push_back(is_vowel_letter(ch) ? L'1' : L'0');
        }
        zero_padding(result, 4);
        auto bcd = card_id + number_id + binary_text_to_bcd(result);
        return bcd_to_sequence(bcd);
    }

    void zero_padding(std::wstring &result, int n) const {
        auto pad = result.size() % n;
        while (pad > 0) {
            result.push_back('0');
            pad = result.size() % n;
        }
    }

    [[nodiscard]] std::wstring binary_text_to_bcd(const std::wstring &text) const {
        std::wstring result;
        for (std::string::size_type i = 0; i < text.size(); i += 4) {
            auto part = text.substr(i, 4);
            unsigned char bits = 0;
            bits = bits | (part[0] == L'0' ? (0 << 3) : (1 << 3));
            bits = bits | (part[1] == L'0' ? (0 << 2) : (1 << 2));
            bits = bits | (part[2] == L'0' ? (0 << 1) : (1 << 1));
            bits = bits | (part[3] == L'0' ? (0 << 0) : (1 << 0));
#if 1
            if (bits > 9)
                continue;
#endif
            result.push_back(bits + L'0');
        }
        return result;
    }

    [[nodiscard]] std::wstring bcd_to_sequence(const std::wstring &bcd) const {
        std::wstring result;
        std::wstring temp = bcd;
        zero_padding(temp, 4);
        for (std::string::size_type i = 0; i < temp.size(); i += 4) {
            auto part = temp.substr(i, 4);
            result += part;
            result += L' ';
        }
        return result;
    }

private:
    [[nodiscard]] constexpr bool is_vowel_letter(int ch) const {
        return ch == letter[0] || ch == letter[1] ||
               ch == letter[2] || ch == letter[3] ||
               ch == letter[4] || ch == letter[5] ||
               ch == letter[6] || ch == letter[7] ||
               ch == letter[8];
    }
};

typedef std::vector<StudentInfo> StudentInfoList;

static std::wstring trim(const std::wstring &str,
                         const std::wstring &whitespace = L" \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::wstring::npos)
        return L"";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

static void usage() {
    std::cout << "Usage: ";
    std::cout << "CourseCalculator filename";
}

static void print_student(std::wostream &wo, const StudentInfoList &students) {
    wo << L"    группа |  зачетка | нпп | ФИО" << std::endl;
    wo << L"---------------------------------" << std::endl;
    for (const auto &at : students) {
        wo << std::setfill(L' ') << std::setw(10) << at.group << " | ";
        wo << at.card_id << L" | ";
        wo << std::setfill(L'0') << std::setw(2) << at.number_id << L"  | ";
        wo << at.name << L" | " << at.job() << std::endl;
    }
}

static std::wstring read_file(const std::string &filename) {
    std::wifstream wif(filename);
    wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << wif.rdbuf();
    return wss.str();
}

static void write_students(const std::string &filename, const StudentInfoList &students) {
    std::wofstream wof(filename);
    wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    print_student(wof, students);
}

static void parse_content(const std::wstring &text, StudentInfoList &students) {
    std::wstring line;
    std::wistringstream input(text);
    while (std::getline(input, line)) {
        int i = 0;
        bool last_space = false;
        std::wstring temp;
        StudentInfo info = {};
        for (wchar_t &it : line) {
            if (it == U' ' && i < 3) {
                if (last_space)
                    continue;
                if (i == 0) {
                    info.group = trim(temp);
                    temp = L"";
                } else if (i == 1) {
                    info.card_id = trim(temp);
                    temp = L"";
                } else if (i == 2) {
                    info.number_id = trim(temp);
                    temp = L"";
                }
                ++i;
                last_space = true;
                continue;
            }
            last_space = false;
            temp.push_back(it);
        }
        info.name = trim(temp);
        students.push_back(std::move(info));
    }
}


int main(int argc, char **argv) {
    std::setlocale(LC_ALL, "");
    if (argc < 2) {
        usage();
        return EXIT_FAILURE;
    }
    StudentInfoList students;
    std::string filename = argv[1];
    std::wstring read = read_file(filename);
    parse_content(read, students);
    std::wcout << L"Студенты: " << std::endl;
    print_student(std::wcout, students);
    write_students("students_result.txt", students);
    return EXIT_SUCCESS;
}
