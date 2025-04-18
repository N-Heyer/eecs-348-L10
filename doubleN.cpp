/*
Author: Nick Heyer
KUID: 3142337
Date: 4/17/25
Lab: 10
Last modified: 4/17/25
EECS 348 Lab 10 
Determine whether a given string represents a valid double-precision number, and then develop functions to perform addition and multiplication on such numbers
input: user inputted file
Output: valid operations done with double prec num and or invalid num is outputted.
Collaborators: 
*/
void trimZeros(string& s, bool leading = true) {
    if (leading) {
        size_t first = s.find_first_not_of('0');
        s = (first == string::npos) ? "0" : s.substr(first);
    } else {
        while (!s.empty() && s.back() == '0') s.pop_back();
    }
}

bool isValidDouble(const string& str) {
    int i = 0;
    if (str[i] == '+' || str[i] == '-') ++i;
    bool hasDigitsBefore = false, hasDot = false, hasDigitsAfter = false;

    while (i < str.size() && isdigit(str[i])) {
        hasDigitsBefore = true;
        ++i;
    }
    if (i < str.size() && str[i] == '.') {
        hasDot = true;
        ++i;
        while (i < str.size() && isdigit(str[i])) {
            hasDigitsAfter = true;
            ++i;
        }
    }
    return (i == str.size()) && (hasDigitsBefore || (hasDot && hasDigitsAfter));
}