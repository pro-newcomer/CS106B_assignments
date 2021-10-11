/*
 * Student: Yifan Yang
 * Section: Wed 6:45
 *
 * This file is a recursion practice that calculates all shortest paths in a
 * grid.
 */
#include <iostream>     // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function takes two arguments `street` and `avenue` and
 * recursively calculate the posible path toward {1, 1}
 */
int countRoutes(int street, int avenue)
{
    if (street == 1 && avenue == 1) {
        return 1;
    }
    if (avenue > 1 && street > 1) {
        return countRoutes(street, avenue - 1) + countRoutes(street - 1, avenue);
    } else if (avenue > 1 && street == 1) {
        return countRoutes(street, avenue - 1);
    } else if (avenue == 1 && street > 1) {
        return countRoutes(street - 1, avenue);
    } else {
        error("Invalid arguments: street and avenue should be >= 1");
    }
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test countRoutes") {
   EXPECT_EQUAL(countRoutes(2, 3), 3);
}

// My test cases here
STUDENT_TEST("Test `countRoutes`") {
   EXPECT_EQUAL(countRoutes(3, 2), 3);
   EXPECT_EQUAL(countRoutes(4, 2), 4);
   EXPECT_ERROR(countRoutes(0, 0));
}
