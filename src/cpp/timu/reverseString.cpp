#include <iostream>
#include <string>
#include <algorithm>

// 反转字符串中的单词顺序，但不反转单词内部的字母顺序
void reverseWords(std::string &s) {
    // 先反转整个字符串
    std::reverse(s.begin(), s.end());

    // 再依次反转每个单词
    int start = 0;
    for (int end = 0; end < s.size(); ++end) {
        if (s[end] == ' ') {
            std::reverse(s.begin() + start, s.begin() + end);
            start = end + 1;
        }
    }
    // 反转最后一个单词
    std::reverse(s.begin() + start, s.end());
}

int main() {
    std::string str = "hello world how are you";

    std::cout << "Original string: " << str << std::endl;

    // 反转字符串中的单词顺序
    reverseWords(str);

    std::cout << "Reversed string with word order preserved: " << str << std::endl;

    return 0;
}
