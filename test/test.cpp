// #define CATCH_CONFIG_MAIN
// #include <catch2/catch_test_macros.hpp>
//
// #include <sstream>
// #include <string>
// #include <vector>
// #include <iostream>
//
// #include "CampusCompass.h"
//
// using namespace std;
//
// static string runCommands(const vector<string>& commands) {
//     CampusCompass compass;
//     REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));
//
//     ostringstream out;
//     streambuf* oldCout = cout.rdbuf(out.rdbuf());
//
//     for (const string& cmd : commands) {
//         compass.ParseCommand(cmd);
//     }
//
//     cout.rdbuf(oldCout);
//     return out.str();
// }
//
// // Required Unit Test 1:
// // At least five incorrect commands
// TEST_CASE("Test 1", "[required][parser]") {
//     vector<string> commands = {
//         "insret \"Mila Hart\" 12345678 1 1 IDS2935",
//         "insert Mila Hart 12345678 1 1 IDS2935",
//         "removeClass",
//         "dropClas 12345678 IDS2935",
//         "replaceClass 12345678 IDS2935"
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// // Required Unit Test 2:
// // At least three edge cases for various functions
// TEST_CASE("Test 2", "[required][edgeCases]") {
//     vector<string> commands = {
//         "remove 87654321",                          // removing student that does not exist
//         "dropClass 87654321 IDS2935",               // drop class for missing student
//         "checkEdgeStatus 777 888",                  // nonexistent edge
//         "insert \"Iris Stone\" 11112222 1 1 IDS2935",
//         "remove 11112222",
//         "remove 11112222"                           // removing same student again
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "DNE\n"
//         "successful\n"
//         "successful\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// // Required Unit Test 3:
// // dropClass, removeClass, remove, and replaceClass
// TEST_CASE("Test 3", "[required][commands]") {
//     vector<string> commands = {
//         "insert \"Riley West\" 23456789 1 3 IDS2935 CIS4301 CNT4007",
//         "replaceClass 23456789 CIS4301 EEL3701",
//         "dropClass 23456789 CNT4007",
//         "removeClass IDS2935",
//         "remove 23456789"
//     };
//
//     string expected =
//         "successful\n"
//         "successful\n"
//         "successful\n"
//         "1\n"
//         "successful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// // Required Unit Test 4:
// // printShortestEdges reachable, then edge closures make it unreachable
// TEST_CASE("Test 4", "[required][printShortestEdges][graph]") {
//     vector<string> commands = {
//         "insert \"Tessa Bloom\" 34567890 1 1 IDS2935",
//         "printShortestEdges 34567890",
//         "toggleEdgesClosure 1 1 2",
//         "toggleEdgesClosure 1 1 4",
//         "printShortestEdges 34567890"
//     };
//
//     string output = runCommands(commands);
//
//     REQUIRE(output.find("successful\n") == 0);
//     REQUIRE(output.find("Time For Shortest Edges: Tessa Bloom\n") != string::npos);
//     REQUIRE(output.find("IDS2935: ") != string::npos);
// }
//
// // Required Unit Test 5:
// // Another strong functional test with invalid then valid operations
// TEST_CASE("Test 5", "[required][validation]") {
//     vector<string> commands = {
//         "insert \"N0ah Reed\" 45678901 1 1 IDS2935",   // bad name
//         "insert \"Noah Reed\" 4567890 1 1 IDS2935",    // bad UFID
//         "insert \"Noah Reed\" 45678901 1 0",           // bad class count
//         "insert \"Noah Reed\" 45678901 1 2 IDS2935",   // mismatch N and class list
//         "insert \"Noah Reed\" 45678901 1 2 IDS2935 CIS4301"
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "successful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 6", "[removeClass][hidden]") {
//     vector<string> commands = {
//         "insert \"Ava Stone\" 91827364 1 3 IDS2935 CIS4301 CNT4007",
//         "insert \"Liam Brooks\" 82736455 1 3 IDS2935 CIS4301 CNT4007",
//         "insert \"Mia Carter\" 73645546 1 3 IDS2935 CIS4301 CNT4007",
//         "insert \"Noah Reed\" 64554637 1 2 IDS2935 CIS4301",
//         "insert \"Emma Price\" 55463728 1 1 EEL3701",
//         "removeClass CNT4007",
//         "removeClass CIS4301",
//         "removeClass EEL3701",
//         "removeClass XYZ9999",
//         "removeClass BAD12"
//     };
//
//     string expected =
//         "successful\n"
//         "successful\n"
//         "successful\n"
//         "successful\n"
//         "successful\n"
//         "3\n"
//         "4\n"
//         "1\n"
//         "unsuccessful\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 7", "[insert][validation][edge]") {
//     vector<string> commands = {
//         "insert \"Nina Patel\" 56473829 1 1 IDS2935",
//         "insert \"Owen Blake\" 56473829 1 1 IDS2935",
//         "insert \"Sara Hill\" 1234567 1 1 IDS2935",
//         "insert \"M4x Ford\" 67584930 1 1 IDS2935",
//         "checkEdgeStatus 777 888"
//     };
//
//     string expected =
//         "successful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "DNE\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 8", "[insert][validation]") {
//     vector<string> commands = {
//         "insert \"Ivy Lane\" 10293847 1 0",
//         "insert \"Kai Moore\" 21039485 1 7 IDS2935 CIS4301 CNT4007 EEL3701 IDS2935 CIS4301 CNT4007",
//         "insert \"Ruby West\" 32049586 1 2 IDS2935",
//         "insert \"Leo Hart\" 43059687 1 2 IDS2935 CIS4301"
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "successful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 9", "[parser][validation]") {
//     vector<string> commands = {
//         "insret \"Tara Quinn\" 99887766 1 1 IDS2935",
//         "insert \"Tara Quinn\" 99887766 1 1 IDS2935",
//         "remvoe 99887766",
//         "remove 99887766"
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "successful\n"
//         "unsuccessful\n"
//         "successful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 10", "[remove]") {
//     vector<string> commands = {
//         "remove 90817263",
//         "insert \"Parker Mills\" 90817263 1 1 IDS2935",
//         "remove 90817263",
//         "remove 90817263"
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "successful\n"
//         "successful\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 11", "[dropClass]") {
//     vector<string> commands = {
//         "dropClass 55112233 IDS2935",
//         "insert \"Chloe Banks\" 55112233 1 2 IDS2935 CIS4301",
//         "dropClass 55112233 CNT4007",
//         "dropClass 55112233 IDS2935",
//         "dropClass 55112233 IDS2935",
//         "dropClass 55112233 CIS4301",
//         "remove 55112233"
//     };
//
//     string expected =
//         "unsuccessful\n"
//         "successful\n"
//         "unsuccessful\n"
//         "successful\n"
//         "unsuccessful\n"
//         "successful\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 12", "[removeClass]") {
//     vector<string> commands = {
//         "insert \"Elena Cross\" 74125896 1 2 IDS2935 CIS4301",
//         "insert \"Jude Flynn\" 85236974 1 1 IDS2935",
//         "removeClass IDS2935",
//         "removeClass IDS2935",
//         "removeClass CIS4301"
//     };
//
//     string expected =
//         "successful\n"
//         "successful\n"
//         "2\n"
//         "unsuccessful\n"
//         "1\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 13", "[removeClass]") {
//     vector<string> commands = {
//         "insert \"Mason Gray\" 66778899 1 1 IDS2935",
//         "removeClass IDS2935",
//         "remove 66778899"
//     };
//
//     string expected =
//         "successful\n"
//         "1\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }
//
// TEST_CASE("Test 14", "[parser]") {
//     vector<string> commands = {
//         "insert \"Nora Dale\" 44332211 1 1 IDS2935",
//         "removeClass IDS2935 extra",
//         "remove 44332211 extra",
//         "dropClass 44332211 IDS2935 extra",
//         "replaceClass 44332211 IDS2935 CIS4301 extra"
//     };
//
//     string expected =
//         "successful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n"
//         "unsuccessful\n";
//
//     REQUIRE(runCommands(commands) == expected);
// }