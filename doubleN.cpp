/*
Author: Nick Heyer
KUID: 3142337
Date: 4/17/25
Lab: 10
Last modified: 4/17/25
EECS 348 Lab 10 
determine whether a given string represents a valid double-precision number, and then develop functions to perform addition and multiplication on such numbers
input: user inputted file
Output: valid operations done with double prec num and or invalid num outputted
Collaborators: 
*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

//gets the sign + or - and returns the rest of the number as a string
pair<char, string> extractSign(const string& s) {
    if (s.empty()) return {'+', ""}; //empty string goes default to positive zero
    if (s[0] == '+' || s[0] == '-') return {s[0], s.substr(1)}; //sign found then return it and the rest
    return {'+', s};//no sign assume +
}

//two strings with leading or trailing 0s to make them the same length
void padZeros(string& a, string& b, bool right = false) {
    int diff = a.size() - b.size();
    if (diff > 0) b.insert(right ? b.size() : 0, abs(diff), '0');
    else if (diff < 0) a.insert(right ? a.size() : 0, abs(diff), '0');
}

//valid whether the inp str is a valid double
bool isValidDouble(const string& s) {
    if (s.empty()) return false;

    int i = 0;
    if (s[i] == '+' || s[i] == '-') i++;

    bool hasDigitsBefore = false;
    while (i < s.size() && isdigit(s[i])) {
        hasDigitsBefore = true;
        i++;
    }

    bool hasDot = false;
    bool hasDigitsAfter = false;
    if (i < s.size() && s[i] == '.') { // decimal
        hasDot = true;
        i++;
        while (i < s.size() && isdigit(s[i])) {
            hasDigitsAfter = true;
            i++;
        }
    }

    if (i != s.size()) return false; //extra char not allowed

    if (!hasDot) return hasDigitsBefore; //if no . must have digits
    return hasDigitsBefore && hasDigitsAfter; //must have digits on both sides of .
}

//compares numeric str by magnitude (ignores signs)
int compareStrings(const string& a, const string& b) {
    if (a.length() != b.length()) return a.length() - b.length(); //compare by length first
    return a.compare(b);
}

//adds two + int str no signs no .
string addStrings(const string& a, const string& b) {
    string res = "";
    int carry = 0;

    for (int i = a.size() - 1; i >= 0; --i) { // right to left +
        int sum = (a[i] - '0') + (b[i] - '0') + carry;
        res += (sum % 10 + '0');
        carry = sum / 10;
    }

    if (carry) res += (carry + '0'); //+ final carry if any
    reverse(res.begin(), res.end()); //reverse the result
    return res;
}

//- b from a a must be >= b both are unsigned int str
string subtractStrings(const string& a, const string& b) {
    string res = "";
    int borrow = 0;

    for (int i = a.size() - 1; i >= 0; --i) {
        int diff = (a[i] - '0') - (b[i] - '0') - borrow;
        if (diff < 0) {
            borrow = 1;
            diff += 10;
        } else {
            borrow = 0;
        }
        res += (diff + '0');
    }

    reverse(res.begin(), res.end()); //reverse the result
    res.erase(0, res.find_first_not_of('0')); //remove leading zeros
    return res.empty() ? "0" : res; //return 0 if result is empty
}

//adds two string represented doubles
string addDoubles(const string& s1, const string& s2) {
    auto [sign1, raw1] = extractSign(s1); //separate sign and number
    auto [sign2, raw2] = extractSign(s2);

    size_t dot1 = raw1.find('.');
    size_t dot2 = raw2.find('.');

    //get int and fract part
    string int1 = (dot1 != string::npos) ? raw1.substr(0, dot1) : raw1;
    string int2 = (dot2 != string::npos) ? raw2.substr(0, dot2) : raw2;
    string frac1 = (dot1 != string::npos) ? raw1.substr(dot1 + 1) : "";
    string frac2 = (dot2 != string::npos) ? raw2.substr(dot2 + 1) : "";

    padZeros(frac1, frac2, true);
    padZeros(int1, int2);

    if (sign1 == sign2) {
        // same sign = reg +
        string frac = addStrings(frac1, frac2);
        bool carry = frac.length() > frac1.length(); //check if fractional addition caused carry
        if (carry) frac = frac.substr(1); //remove carry digit from fractional part
        string integer = addStrings(int1, int2);
        if (carry) integer = addStrings(integer, "1"); //add carry to integer part

        return string(1, sign1) + integer + "." + frac;
    } else {
        //opposite signs means to perform -
        string full1 = int1 + frac1;
        string full2 = int2 + frac2;
        padZeros(full1, full2); //align total lengths

        int cmp = compareStrings(full1, full2); //compare str
        string resFrac, resInt, signResult;

        if (cmp == 0) {
            return "+0.0"; //num cancel out
        } else if (cmp > 0) {
            resFrac = subtractStrings(frac1, frac2);
            if (frac1 < frac2) {
                int1 = subtractStrings(int1, "1");
                frac1 = addStrings(frac1, string(frac1.size(), '9'));
                frac1[frac1.size() - 1]++;
                resFrac = subtractStrings(frac1, frac2);
            }
            resInt = subtractStrings(int1, int2);
            signResult = string(1, sign1);
        } else {
            resFrac = subtractStrings(frac2, frac1);
            if (frac2 < frac1) {
                int2 = subtractStrings(int2, "1");
                frac2 = addStrings(frac2, string(frac2.size(), '9'));
                frac2[frac2.size() - 1]++;
                resFrac = subtractStrings(frac2, frac1);
            }
            resInt = subtractStrings(int2, int1);
            signResult = string(1, sign2);
        }

        return signResult + resInt + "." + resFrac;
    }
}

int main() {
    string filename;
    cout << "Enter file name: ";
    getline(cin, filename); //read filename from user

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file.\n";
        return 1;
    }

    string line;
    string constant = "-123.456"; //constant to add to each num thats valid
    while (getline(file, line)) {
        if (!isValidDouble(line)) {
            cout << "Invalid double: " << line << endl;
        } else {
            string result = addDoubles(line, constant);
            cout << "Result of " << line << " + " << constant << " = " << result << endl;
        }
    }

    file.close();
    return 0;
}
