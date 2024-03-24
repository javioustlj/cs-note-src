#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <map>



class Solution {
public:
    std::string intToRoman(int num) {
        if (num <= 0) return "";

        std::map<int, std::string> roman_numerals = {
            {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
            {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"},
            {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"}, {1, "I"}
        };

        std::string result;
        for (auto it = roman_numerals.crbegin(); it != roman_numerals.crend(); ++it) {
            while (num >= it->first) {
                num -= it->first;
                result += it->second;
            }
            if(num == 0)
                break;
        }

        return result;
    };
    std::string intToRoman2(int num) {
        const std::pair<int, std::string> valueSymbols[] = {
            {1000, "M"},
            {900,  "CM"},
            {500,  "D"},
            {400,  "CD"},
            {100,  "C"},
            {90,   "XC"},
            {50,   "L"},
            {40,   "XL"},
            {10,   "X"},
            {9,    "IX"},
            {5,    "V"},
            {4,    "IV"},
            {1,    "I"},
        };
        std::string roman;
        for (const auto &[value, symbol] : valueSymbols) {
            while (num >= value) {
                num -= value;
                roman += symbol;
            }
            if (num == 0) {
                break;
            }
        }
        return roman;
    }
};

int main() {
    Solution solution;
    int number;
    std::cin >> number;
    std::string roman = solution.intToRoman(number);
    std::cout << "The Roman numeral for " << number << " is " << roman << std::endl;
    std::string roman2 = solution.intToRoman2(number);
    std::cout << "The Roman numeral for " << number << " is " << roman2 << std::endl;
    return 0;
}
