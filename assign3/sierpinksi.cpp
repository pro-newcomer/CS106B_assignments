/*
 * Student: Yifan Yang
 * Section: Wed 6:45
 *
 * This file contains drawing method for fractal shape -- sierpinksi. using recursion
 *
 * And An Extension!!!!
 * This extention is contructed using a recursion function and can be run in the interactive
 * demo. it is a snow flake fractal starting with a equilateral triangle. It only needs 2 input
 * parameters as one side of the equiliteral triangle. I've overloaded operators for struct GPoint
 * and then the GPoint could be used intuitively in code as written in mathematics. GPoints are now
 * complex number or vectors(in scalar multiplication and addition/subtraction). Therefore, some
 * calculation tricks could be made to manipulate these points in a complex plane like rotation by
 * some angle or convex linear combinations to get anker points on the vectors
 *
 * Noteworthy:
 * During debug, I found that the frame of the coordinate system in the graphic window is different from
 * the normal x-y frame: the frame in the graphic window points x direction to the right and y direction
 * to the bottom. Therefore, I have to take this reversed frame into account when defining the rotation vector
 * and the triangle side vector's direction.
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

// Make intuitive operators available for GPoints
// Therefore, with these operators support, I could treat GPoints like complex numbers in math.
// It is basically useful for the extension, where I tried to rotate a vector.
inline GPoint operator+(const GPoint &lhs, const GPoint &rhs) {
    return GPoint((lhs.x + rhs.x), (lhs.y + rhs.y));
}

// Divided by a scalar. Could be replaced by the multiplication but kept for comvenience
inline GPoint operator/(const GPoint &lhs, const int &rhs) {
    return GPoint((lhs.x / rhs), (lhs.y / rhs));
}

// Minus operators to do vector subtraction or complex num subtraction
inline GPoint operator-(const GPoint &lhs, const GPoint &rhs) {
    return GPoint(lhs.x - rhs.x, lhs.y - rhs.y);
}

// unary operators to negate vectors
inline GPoint operator-(const GPoint &rhs) {
    return GPoint(-rhs.x, -rhs.y);
}

// complex number multiplication
inline GPoint operator*(const GPoint &lhs, const GPoint &rhs) {
    return GPoint(lhs.x * rhs.x - lhs.y * rhs.y, lhs.y * rhs.x + lhs.x * rhs.y);
}

// scalar multiplication
inline GPoint operator*(const int &lhs, const GPoint &rhs) {
    return GPoint(lhs * rhs.x, lhs * rhs.y);
}

inline GPoint operator*(const GPoint &lhs, const int &rhs) {
    return GPoint(lhs.x * rhs, lhs.y * rhs);
}

inline GPoint operator*(const double &lhs, const GPoint &rhs) {
    return GPoint(lhs * rhs.x, lhs * rhs.y);
}

inline GPoint operator*(const GPoint &lhs, const double &rhs) {
    return GPoint(lhs.x * rhs, lhs.y * rhs);
}

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

/*
 * This function draws a fractal triangle in the given `window` and the 3 points defined in the
 * give `order` and returns the total number of triangle drawn
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    if (order == 0) {
        fillBlackTriangle(window, one, two, three);
        return 1;
    } else {
        return drawSierpinskiTriangle(window, one  , (one + two) / 2  , (one + three) / 2, order - 1) +
               drawSierpinskiTriangle(window, two  , (two + one) / 2  , (two + three) / 2, order - 1) +
               drawSierpinskiTriangle(window, three, (three + one) / 2, (three + two) / 2, order - 1);
    }
}


// Extension here:

/* Since I could not upload the header file nor edit the interactive demo function in GUI file
 * I made this function the same name as the required one. To see this extension, you could
 * comment the function above and uncomment this one to run it in the interactive demo.
 *
 * The input is made identical to the function above because of the header definition
 * but actually there is only 2 points needed. Therefore, the 3rd param GPoint is marked as `_`
 * which means it is not used and the input of that point does not affect the graphics.
 *
 * You could see GPiont calculations with GPoint. The operators with GPoint are redefined by me at
 * the beginning of this file to make it more intuitive and compatible with `vectors` or
 * `complex numbers` in mathematics.
 *
 * The recursion return of the function uses const `1` `3` and `4` to define the 1/4 and 3/4 points
 * on a given side of the triangle, which uses the idea of convex linear combination
 */
//int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint _, int order) {
//    // make a equilateral triangle
//    GPoint rotatingVector = {0.5, sqrt(3) / 2}; // 60 deg clockwise: `e to the i pi over three` e^{iπ / 3} or (cos(π/3) + isin(π/3))
//    GPoint sideVector = one - two;
//    GPoint three = two + (sideVector * rotatingVector);
//    fillBlackTriangle(window, one, two, three);
//    if (order == 0) {
//        return 1;
//    } else {
//        // grow next order triangles on each side of this triangle
//        return drawSierpinskiTriangle(window, (1 * one + 3 * two  ) / 4, (3 * one + 1 * two  ) / 4, _, order - 1) +
//               drawSierpinskiTriangle(window, (1 * two + 3 * three) / 4, (3 * two + 1 * three) / 4, _, order - 1) +
//               drawSierpinskiTriangle(window, (1 * three + 3 * one) / 4, (3 * three + 1 * one) / 4, _, order - 1);
//    }
//}


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}

