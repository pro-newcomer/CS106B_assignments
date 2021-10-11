/*
 * Student: Yifan Yang
 * Section: Wed 6:45
 *
 * This file contains code that implements the queue-merging algorithms.
 * And some tests are done to study the recursion and iteration's Big O
 * Learned a lot about optimization and algorithm designs.
 */
#include <iostream>    // for cout, endl
#include "queue.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function takes 2 sorted queue and returns a combined sorted
 * queue.
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    Queue<int> result;
    if (a.isEmpty()) {
        return b;
    } else if (b.isEmpty()) {
        return a;
    }
    int a_first;
    int b_first;
    while (!a.isEmpty() && !b.isEmpty()) {
        a_first = a.peek();
        b_first = b.peek();
        if (a_first <= b_first) {
            result.enqueue(a.dequeue());
        } else {
            result.enqueue(b.dequeue());
        }
        if ((!a.isEmpty() && a.peek() < a_first) || (!b.isEmpty() && b.peek() < b_first)) {
            error("Input array not in order");
        }
    }
    while (!a.isEmpty()) {
        a_first = a.peek();
        result.enqueue(a.dequeue());
        if ((!a.isEmpty() && a.peek() < a_first)) {
            error("Input array not in order");
        }
    }
    while (!b.isEmpty()) {
        b_first = b.peek();
        result.enqueue(b.dequeue());
        if ((!b.isEmpty() && b.peek() < b_first)) {
            error("Input array not in order");
        }
    }
    return result;
}

/*
 * This function assumes correct functionality of the previously
 * defined binaryMerge function and makes use of this function to
 * iteratively merge a collection of sequences, starting off with
 * an empty sequence and progressively merging in one sequence at
 * a time. This function is provided for you fully implemented –
 * it does not need to modified at all.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

/*
 * This function takes in a Vector of sorted queues and run recursively
 * to merge all the queues into a single sorted queue.
 */
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;
    if (all.size() < 2) {
        return all.isEmpty() ? result : all[0];
    } else if (all.size() == 2) {
        return binaryMerge(all[0], all[1]);
    } else {
        int mid = all.size() / 2;
        Vector<Queue<int>> first = all.subList(0, mid);
        Vector<Queue<int>> second = all.subList(mid);
        return binaryMerge(recMultiMerge(first), recMultiMerge(second));
    }
}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}

// My student test here
STUDENT_TEST("Time the operation `binaryMerge`") {
    for (int i = 2000000; i < 2000000 * pow(2, 3); i *= 2) {
        Queue<int> a = createSequence(i);
        Queue<int> b = createSequence(i);
        TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
    }
}

STUDENT_TEST("Test `naiveMultiMerge` behavior") {
    Vector<Queue<int>> input;
    EXPECT_EQUAL(naiveMultiMerge(input), {});
    input = {{}, {}, {}};
    EXPECT_EQUAL(naiveMultiMerge(input), {});
}

STUDENT_TEST("Time operation of `naiveMultiPage` Big O with fixed n") {
    int n = 200000;
    for (int k = 4; k < 7; k++) {
        Vector<Queue<int>> input;
        for (int i = 0; i < k; i++) {
            input += createSequence(n / k);
        }
        TIME_OPERATION(k, naiveMultiMerge(input));
    }
}

STUDENT_TEST("Time operation of `naiveMultiPage` Big O with fixed k") {
    int k = 5;
    for (int n = 100000; n < 400000; n += 100000) {
        Vector<Queue<int>> input;
        for (int i = 0; i < k; i++) {
            input += createSequence(n / k);
        }
        TIME_OPERATION(n, naiveMultiMerge(input));
    }
}

STUDENT_TEST("Test `recMultiMerge` behavior") {
    int k = 5;
    for (int n = 100000; n < 400000; n += 100000) {
        Vector<Queue<int>> input;
        for (int i = 0; i < k; i++) {
            input += createSequence(n / k);
        }
        EXPECT_EQUAL(recMultiMerge(input), naiveMultiMerge(input));
    }
}

STUDENT_TEST("Time operation of `recMultiPage` Big O with fixed n") {
    int n = 400000;
    for (int k = 4; k <= 4 * 2 * 2; k *= 2) {
        Vector<Queue<int>> input;
        for (int i = 0; i < k; i++) {
            input += createSequence(n / k);
        }
        TIME_OPERATION(k, recMultiMerge(input));
    }
}

STUDENT_TEST("Time operation of `recMultiPage` Big O with fixed k") {
    int k = 5;
    for (int n = 100000; n < 400000; n += 100000) {
        Vector<Queue<int>> input;
        for (int i = 0; i < k; i++) {
            input += createSequence(n / k);
        }
        TIME_OPERATION(n, recMultiMerge(input));
    }
}
