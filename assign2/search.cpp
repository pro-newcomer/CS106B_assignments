/*
 * Student: Yifan Yang
 * Section: Wed 6:45
 *
 * In this file, inverted indexing algorithm is implemented and we
 * built a console search engine.
 *
 * I used a little trick in the user-friendly out put with some c++
 * functions from <chrono>. But I stayed in the bound of the course when
 * implementing the algorithms.
 *
 * There are some interesting stuff at the end of the file
 */
#include <iostream>
#include <fstream>
#include <chrono>
#include <regex>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "strlib.h"
#include "vector.h"
#include "simpio.h"
#include "testing/SimpleTest.h"
using namespace std;
using namespace std::chrono;

/* This funtion takes an argument `s` and returns a
 * new string where punctuations are stripped from
 * both sides of the string.
 * It goes from the start of the string to find the first
 * non-punct char and store its index.
 * Then it goes from the end of the string to the start of the
 * string to find the first non-punct and stops if the index hits
 * `start` index.
 * Eventually, it returns the substr of the recorded index.
 */
string trimPunct(string& s) {
    int start = 0;
    int end = s.length() - 1;
    while (start < s.length() && ispunct(s[start])) {
        start++;
    }
    while (end >= start && ispunct(s[end])) {
        end--;
    }
    return s.substr(start, end - start + 1);
}


/* This function takes an argument `s` and returns a string that
 * is "cleaned"--trimmed punctuations, checked to have at least 1
 * letter and converted to lowercase.
 */
string cleanToken(string s)
{
    s = trimPunct(s);
    bool hasLetters = false;
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            hasLetters = true;
        }
    }
    if (!hasLetters) {
        return "";
    }
    toLowerCaseInPlace(s);
    return s;
}

/* This function takes a string of body text and return all the terms in it.
 * Terms are all cleaned and unique in the returned set.
 * The function iterates through the splitted list of words to check if the cleaned
 * token valid and add it to the returned set.
 */
Set<string> gatherTokens(string text)
{
    Set<string> tokens;
    for (string word : stringSplit(text, " ")) {
        string cleaned = cleanToken(word);
        if (cleaned != "") {
            tokens.add(cleaned);
        }
    }
    return tokens;
}

/* This function takes two args `dbfile` and `index` and return the indexed
 * map of the dbfile. It iterates throught the file lines and gather tokens
 * and index them with the url while adding them to the map.
 * Finally, it returns the total number of pages processed
 */
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines;
    readEntireFile(in, lines);

    int numPageProcessed = 0;

    for (int i = 0; i < lines.size(); i += 2) {
        string url = lines[i];
        string body = lines[i + 1];
        Set<string> tokens = gatherTokens(body);
        for (string token : tokens) {
            index[token].add(url);
        }
        numPageProcessed++;
    }
    return numPageProcessed;
}

/* This function parses the input `query` and search for mathes in the given
 * `index` and return the matches found. It first split the query string and
 * parse the "+" or "-" sign then clean the terms before searching result.
 */
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)
{
    Set<string> result;

    Vector<string> terms = stringSplit(query, " ");
    for (string term : terms) {
        if (term[0] == '+') {
            result.intersect(index[cleanToken(term)]);
        } else if (term[0] == '-') {
            result.difference(index[cleanToken(term)]);
        } else {
            result.unionWith(index[cleanToken(term)]);
        }
    }
    return result;
}

/* This function is a console program of this search engine.
 * It takes an arg `dbfile` which is the data base gonna be
 * used in this process. It first index the db and ask for user input.
 * Users could enter their search queries and get matching reult
 * or press ENTER to quit the program.
 */
void searchEngine(string dbfile) {
    Map<string, Set<string>> index;
    cout << "Stand by while building index..." << endl;
    // a little trick showing the time of indexing operation
    // start a timer
    auto start = high_resolution_clock::now();
    // build index
    int numPages = buildIndex("res/website.txt", index);
    // stop timer and calculate duration
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    // Welcome output
    cout << "Indexed " << numPages << " pages containing "
         << index.size() << " unique terms in " << duration.count() / 1000.0
         << " seconds :)\n" << endl;
    string userInput = getLine("Enter query sentence (RETURN/ENTER to quit): ");
    while (userInput != "") {
        Set<string> result = findQueryMatches(index, userInput);
        // user-friendly output for 0 result cases
        if (result.size() == 0) {
            cout << "No matches found. Try something else?\n" << endl;
        } else {
            // standard output. See `advancedSearchEngine` for beautiful output
            cout << "Found " << result.size() << " matching pages.\n"
                 << result << "\n" << endl;
        }
        userInput = getLine("Enter query sentence (RETURN/ENTER to quit): ");
    }

    // Prompt if the user wants to try extended feature in my assignment
    cout << "SearchEngine Stopped.\n" << endl;
    userInput = getLine("Would you like to try an Advanced Version(AdvancedSearchEngine)[Y/n]?");
    // Parse user input and redirect to advanced Engine
    if (userInput == "Y" || userInput == "y" || userInput == "") {
        advancedSearchEngine(dbfile);
    }
}

/* -------------------------------- */
/* --------- FBI  WARNING --------- */
/* -------------------------------- */
/* ---- EXTENSIONS STARTS HERE ---- */
/* -------------------------------- */

/* This function is an advanced version of `gatherTokens`. It not
 * only indexes the url of the token's occurrence, but also indexes
 * the position of the token in that page.
 */
Map<string, Set<int>> gatherAdvancedTokens(string text) {
    Map<string, Set<int>> tokens;
    Vector<string> words = stringSplit(text, " ");
    for (int i = 0; i < words.size(); i++) {
        string cleaned = cleanToken(words[i]);
        if (cleaned != "") {
            tokens[cleaned].add(i);
        }
    }
    return tokens;
}

/* This function is an advanced version of the function `buildIndex`.
 * It populates an index that has the word's exact position in pages.
 */
int buildAdvancedIndex(string dbfile, Map<string, Map<string, Set<int>>>& index) {
    ifstream in;
    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    // read file into vector
    Vector<string> lines;
    readEntireFile(in, lines);

    // init counter
    int numPagesProcessed = 0;

    // start indexing
    for (int i = 0; i < lines.size(); i += 2) {
        // Parse url and content
        string url = lines[i];
        string body = lines[i + 1];
        // gather tokens
        Map<string, Set<int>> tokens = gatherAdvancedTokens(body);
        for (string token : tokens) {
            index[token][url] = tokens[token];
        }
        // counter update
        numPagesProcessed++;
    }
    // return counter
    return numPagesProcessed;
}

/* Specific substitution for Set.intersect for two Map keys
 */
void intersectMap(Map<string, int>& weightedResult, Map<string, Set<int>>& resultUrls) {
    // cannot modify a collection in a for-each loop of that collection
    // so store the keys in a vector and for-each this vector
    Vector<string> existingUrls = weightedResult.keys();
    for (string url : existingUrls) {
        if (resultUrls.containsKey(url)) {
            weightedResult[url] += resultUrls[url].size();
        } else {
            weightedResult.remove(url);
        }
    }
}

/* Specific substitution for Set.difference for two Map keys
 */
void differenceMap(Map<string, int>& weightedResult, Map<string, Set<int>>& resultUrls) {
    for (string url : resultUrls) {
        if (weightedResult.containsKey(url)) {
            weightedResult.remove(url);
        }
    }
}

/* Specific substitution for Set.unionWith for two Map keys
 */
void unionWithMap(Map<string, int>& weightedResult, Map<string, Set<int>>& resultUrls) {
    for (string url : resultUrls) {
        weightedResult[url] += resultUrls[url].size();
    }
}

/* This function is for extracting phrases from user query to perform
 * phrase search. It uses regex to extract text in double quotes.
 *
 * To do a phrase search:
 * "function call"
 * searches for the exact phrase `function call`
 *
 * Plus/minus sign supported
 */
Vector<string> gatherPhrases(string query) {
    Vector<string> phrases;
    // regular expression:
    // - probably start with +/-
    // - wrapped by double quotes
    regex rgx("[\\+\\-]?\"[^\"]*\"");
    sregex_iterator cur(query.begin(), query.end(), rgx);
    sregex_iterator end;
    // find all matches and add them to the vector in order.
    while (cur != end) {
        smatch match = *cur;
        string matchStr = match.str();
        phrases.add(matchStr);
        cur++;
    }
    return phrases;
}

/* Check if a word is a phrase.
 */
bool isPhrase(string s) {
    return s[0] == '\"' && s[s.length() - 1] == '\"';
}

/* This function helps search a phrase result in the given index. It depends on the advanced
 * index which have all the occurrences of a word and its position.
 */
Map<string, Set<int>> searchPhrase(Map<string, Map<string, Set<int>>>& index, string p) {
    Map<string, Set<int>> resultUrls;
    // Remove the quote from the phrase
    size_t pos;
    while ((pos = p.find('\042')) != string::npos) {
        p.erase(pos, 1);
    }
    // split the components of this phrase to do search and combine them
    Vector<string> components = stringSplit(p, " ");
    resultUrls = index[cleanToken(components[0])];
    for (int i = 1; i < components.size(); i++) {
        Map<string, Set<int>> tempRes = index[cleanToken(components[i])];
        // Check if they are next to each other and merge the set.
        for (string url : tempRes) {
            if (resultUrls.containsKey(url)) {
                Set<int> newIndexSet;
                for (int prev_pos : resultUrls[url]) {
                    for (int curr_pos : tempRes[url]) {
                        if (curr_pos - prev_pos == 1) {
                            // The word is next to the previous word
                            newIndexSet.add(curr_pos);
                        }
                    }
                }
                if (newIndexSet.size() == 0) {
                    // if no result, remove from the list
                    resultUrls.remove(url);
                } else {
                    // if there are result, replace the set
                    resultUrls[url] = newIndexSet;
                }
            } else {
                // not in a same page, remove url
                resultUrls.remove(url);
            }
        }
    }
    return resultUrls;
}

/* This function does the advanced search on the index. Since the data structure changed, it
 * could not use the intersect/difference/unionWith method of `Set`. I wrote 3 specific functions
 * for this use case and completed the task.
 */
Map<string, int> findAdvancedQueryMatches(Map<string, Map<string, Set<int>>>& index, string query) {
    Map<string, int> weightedResult;

    // TODO Build phrase search
    Vector<string> phrases = gatherPhrases(query);
    Vector<string> terms;
    if (phrases.size() > 0) {
        // if there are phrases, the query string should be parsed differently
        // to keep the compound order
        for (string phrase : phrases) {
            int phrasePos = query.find(phrase);
            terms.addAll(stringSplit(query.substr(0, phrasePos), " "));
            terms.add(phrase);
            query = query.substr(phrasePos + phrase.length());
        }
    } else {
        terms = stringSplit(query, " ");
    }
    // now `term` has all terms and "phrase terms" in it and with original order
    for (string term : terms) {
        Map<string, Set<int>> resultUrls;
        if (term[0] == '+') {
            // remove the operator
            string t = term.substr(1);
            if (isPhrase(t)) {
                resultUrls = searchPhrase(index, t);
            } else {
                resultUrls = index[cleanToken(term)];
            }
            intersectMap(weightedResult, resultUrls);
        } else if (term[0] == '-') {
            // remove the operator
            string t = term.substr(1);
            if (isPhrase(t)) {
                resultUrls = searchPhrase(index, t);
            } else {
                resultUrls = index[cleanToken(term)];
            }
            differenceMap(weightedResult, resultUrls);
        } else {
            string t = term;
            if (isPhrase(t)) {
                resultUrls = searchPhrase(index, t);
            } else {
                resultUrls = index[cleanToken(term)];
            }
            unionWithMap(weightedResult, resultUrls);
        }
    }
    return weightedResult;
}

/* This function sorts the raw search result with relevance index and
 * returns a sorted Vector that contains the urls. Other functions
 * may use the returned result to get values from the raw result
 * in order.
 */
Vector<string> sortResultByRelevance(Map<string, int> raw) {
    // A simple selection sort instead of other fancy sorting algorithms
    // I suppose there won't be too much items to sort currently
    // Also, this implementation can be easily rewritten if necessary
    Vector<string> sorted;
    while (raw.keys().size() != 0) {
        int max = raw[raw.keys()[0]];
        int maxIndex = 0;
        for (int i = 0; i < raw.keys().size(); i++) {
            if (raw[raw.keys()[i]] > max) {
                max = raw[raw.keys()[i]];
                maxIndex = i;
            }
        }
        sorted.add(raw.keys()[maxIndex]);
        raw.remove(raw.keys()[maxIndex]);
    }
    return sorted;
}

/* This function is a helper function to help printing out the query result in
 * a beautiful way. It composes a string and then use cout to output it to the
 * console
 */
void printPrettyResult(Map<string, int>& result, Vector<string>& sorted) {
    string header = "Rank \tRelevance Index \tURL\n";
    string divider = "---- \t--------------- \t--------------------\n";
    string table = "";
    string bottom = "---------------------------------------------------\n";
    // populate table content
    for (int i = 0; i < sorted.size(); i++) {
        // rank field fixed width 3
        string rank = to_string(i + 1) + "  \t";
        if (i < 9) rank = "0" + rank;
        if (i < 99) rank = "0" + rank;
        int rel = result[sorted[i]];
        // relevance index field width
        string relevance = to_string(rel) + "            ";
        if (rel < 10) relevance += " ";
        if (rel < 100) relevance += " ";
        relevance += " \t";
        // url field
        string url = sorted[i];
        // add the string row to the table
        table += rank + relevance + url + "\n";
    }
    cout << header + divider + table + bottom << endl;
}

/* Cool Trick Here!!!
 *
 * Just add this line to `search.h` header and change `searchEngine` in `main.cpp`
 * to `advancedSearchEngine` to try this out!!!
 *
 * This program is much more interactive and user-friendly compared to the `searchEngine`
 * I wrote a function to sort the result by its relevance and another function to print
 * it out as a well formatted table!
 */
void advancedSearchEngine(string dbfile) {
    Map<string, Map<string, Set<int>>> index;
    cout << "Stand by while building index..." << endl;
    // a little trick showing the time of indexing operation
    auto start = high_resolution_clock::now();
    int numPages = buildAdvancedIndex("res/website.txt", index);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Indexed " << numPages << " pages containing " <<
            index.size() << " unique terms in " << duration.count() / 1000.0 << " seconds :)\n" << endl;
    // Store the search history for final report
    Vector<string> searchHistory;
    string userInput = getLine("Enter query sentence (RETURN/ENTER to quit): ");
    while (userInput != "") {
        searchHistory.add(userInput); // record search
        // start timer
        start = high_resolution_clock::now();
        // Do the search
        Map<string, int> rawResult = findAdvancedQueryMatches(index, userInput);
        Vector<string> result = sortResultByRelevance(rawResult);
        // stop the timer and calculate duration
        stop = high_resolution_clock::now();
        auto searchDuration = duration_cast<microseconds>(stop - start);
        // User friendly output
        if (result.size() == 0) {
            cout << "No matches found. Try something else?\n" << endl;
        } else {
            cout << "Found " << result.size() << " matching pages in "
                 << searchDuration.count() / 1000.0 << " ms. Sorted by relevance: "
                 << endl;
            printPrettyResult(rawResult, result);
        }
        userInput = getLine("Enter query sentence (RETURN/ENTER to quit): ");
    }
    // Quiting
    cout << "\nDone " << searchHistory.size() << " searches." << endl;
    cout << "Thank you for choosing AdvancedSearchEngine! Good day!" << endl;
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings with no punctuation at beginning or end") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS*106B"), "cs*106b");
}

PROVIDED_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word strings (no letters)"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs*106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 5);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


// My student test cases here

STUDENT_TEST("Test func `trim`") {
    string s = "<<>!";
    EXPECT_EQUAL(trimPunct(s), "");
}

STUDENT_TEST("Test `cleanToken`") {
    string s = "--+=TestToken//";
    EXPECT_EQUAL(cleanToken(s), "testtoken");
    s = "#Test-Token_example{}";
    EXPECT_EQUAL(cleanToken(s), "test-token_example");
}

STUDENT_TEST("Test `gatherTokens`") {
    string s = "Time to test! Add test cases that confirm the output from gatherTokens";
    EXPECT_EQUAL(gatherTokens(s).size(), 11);
    s = "TheSe are $pecia1 Cases ${cases}";
    EXPECT_EQUAL(gatherTokens(s).size(), 4);
    s = "cs106B cslO6b cslO6B C51O6B CS106B CSl06B";
    EXPECT_EQUAL(gatherTokens(s).size(), 4);
}

STUDENT_TEST("Test `buildIndex`") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    EXPECT_EQUAL(nPages, 32);
    EXPECT_EQUAL(index.size(), 3875);
    EXPECT(index.containsKey("duplicatenegatives"));
    TIME_OPERATION(index.size(), buildIndex("res/website.txt", index));
}

STUDENT_TEST("Test `findQueryMatches` from tiny.txt, misc queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesMilk = findQueryMatches(index, "milk");
    EXPECT_EQUAL(matchesMilk.size(), 1);
    Set<string> matchesBlueAndGreen = findQueryMatches(index, "blue +green");
    EXPECT_EQUAL(matchesBlueAndGreen.size(), 1);
    Set<string> matchesFishWithoutTwo = findQueryMatches(index, "fish -two");
    EXPECT_EQUAL(matchesFishWithoutTwo.size(), 2);
    Set<string> matchesFishWithoutMilkOrBlue = findQueryMatches(index, "fish -milk blue");
    EXPECT_EQUAL(matchesFishWithoutMilkOrBlue.size(), 3);
}

/* -------------------------------- */
/* --------- FBI  WARNING --------- */
/* -------------------------------- */
/* ---- EXTENSIONS STARTS HERE ---- */
/* -------------------------------- */


STUDENT_TEST("Test advanced `gatherAdvancedTokens` function") {
    Map<string, Set<int>> expected = {{"go", {0, 1, 2}}, {"gophers", {3}}};
    EXPECT_EQUAL(gatherAdvancedTokens("go go go gophers"), expected);
    expected = {{"i", {0}}, {"love", {1}}, {"cs*106b", {2}}};
    EXPECT_EQUAL(gatherAdvancedTokens("I _love_ CS*106B!"), expected);
}

STUDENT_TEST("Test advanced `buildAdvancedIndex` function") {
    Map<string, Map<string, Set<int>>> index;
    int nPages = buildAdvancedIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    cout << index<< endl;
    EXPECT_EQUAL(index.keys().size(), 11);
    EXPECT(index.containsKey("fish"));
    EXPECT_EQUAL(index["fish"]["www.dr.seuss.net"], {1, 3, 5, 7, 11});
}

STUDENT_TEST("Test gatherPhrases") {
    string s = "hello \"Yifan Yang\" +\"morning ya\" -\"function\" thank you";
    EXPECT_EQUAL(gatherPhrases(s), {"\"Yifan Yang\"", "+\"morning ya\"", "-\"function\""});
}

// Other extension functions are tested in the console program and was verified
// to have the desired behavior after referring to the standard console program.
