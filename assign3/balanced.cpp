/*
 * Student: Yifan Yang
 * Section: Wed 6:45
 *
 * This file contains code that implements the brackets closing check
 * function that check if the bracketing operators are in pairs and nested
 * correctly.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "testing/SimpleTest.h"

using namespace std;

/* Check if a char is a bracketing operator.
 * Helper method for more readability
 */
bool isBracket(char s) {
    string allOperators = "()[]{}";
    return allOperators.find(s) != string::npos;
}

/*
 * This function takes an argument `str` and returns the sequence of
 * bracketing operators in that string in a recursive implementation.
 */
string operatorsFrom(string str) {
    if (str.empty()) {
        return "";
    }
    if (isBracket(str[0])) {
        return str[0] + operatorsFrom(str.substr(1));
    } else {
        return operatorsFrom(str.substr(1));
    }
}

/*
 * This function takes an argument `ops` that is all bracketing operators
 * and check if the operators are matched.
 */
bool operatorsAreMatched(string ops) {
    if (ops.empty()) {
        return true;
    }
    // This for-each is for shortening the code
    for (string pair : {"()", "[]", "{}"}) {
        int pos = ops.find(pair);
        if (pos != string::npos) {
            return operatorsAreMatched(ops.replace(pos, 2, ""));
        }
    }
    return false;
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

// my student test cases

STUDENT_TEST("Test `operatorsFrom`") {
    EXPECT_EQUAL(operatorsFrom("string operatorsFrom(string str) {"), "(){");
    EXPECT_EQUAL(operatorsFrom("string operatorsFrom"), "");
    EXPECT_EQUAL(operatorsFrom("onRefresh => () async { setState(() { data.clear() }) }"), "(){((){()})}");
}

STUDENT_TEST("Test `operatorsAreMatched`") {
    EXPECT(operatorsAreMatched("(){((){()})}"));
    EXPECT(operatorsAreMatched(""));
    EXPECT(!operatorsAreMatched("({)[]}"));
    EXPECT(!operatorsAreMatched("("));
}
