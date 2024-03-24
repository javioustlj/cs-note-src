const pair<int, string> valueSymbols[] = {
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

class Solution {
public:
    string intToRoman(int num) {
        string roman;
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
    std::cout << "The Roman numeral for " << number << " is " << roman << std::endl;
    return 0;
}
