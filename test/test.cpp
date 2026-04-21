#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "CampusCompass.h"

using namespace std;

static string runCommands(const vector<string>& commands) {
    CampusCompass compass;
    REQUIRE(compass.ParseCSV("data/edges.csv", "data/classes.csv"));

    ostringstream out;
    streambuf* oldCout = cout.rdbuf(out.rdbuf());

    for (const string& cmd : commands) {
        compass.ParseCommand(cmd);
    }

    cout.rdbuf(oldCout);
    return out.str();
}

TEST_CASE("Test 07 hidden style: removeClass edge cases", "[removeClass][hidden]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 3 COP3530 CDA3101 MAC2311",
        "insert \"John Doe\" 22222222 1 3 COP3530 CDA3101 MAC2311",
        "insert \"John Doe\" 33333333 1 3 COP3530 CDA3101 MAC2311",
        "insert \"John Doe\" 44444444 1 2 COP3530 CDA3101",
        "insert \"John Doe\" 55555555 1 1 PHY2048",
        "removeClass MAC2311",
        "removeClass CDA3101",
        "removeClass PHY2048",
        "removeClass CEN3031",
        "removeClass ABC1234"
    };

    string expected =
        "successful\n"
        "successful\n"
        "successful\n"
        "successful\n"
        "successful\n"
        "3\n"
        "4\n"
        "1\n"
        "unsuccessful\n"
        "unsuccessful\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Test 08 hidden style: unique IDs, 8 digit IDs, bad names, nonexistent edge", "[insert][validation][edge]") {
    vector<string> commands = {
        "insert \"Alice Smith\" 12345678 1 1 COP3530",
        "insert \"Bob Jones\" 12345678 1 1 COP3530",
        "insert \"Carol\" 1234567 1 1 COP3530",
        "insert \"D4vid\" 12345679 1 1 COP3530",
        "checkEdgeStatus 999 1000"
    };

    string expected =
        "successful\n"
        "unsuccessful\n"
        "unsuccessful\n"
        "unsuccessful\n"
        "DNE\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Test 09 hidden style: insert validation for N range and count match", "[insert][validation]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 0",
        "insert \"John Doe\" 22222222 1 7 COP3530 CDA3101 MAC2311 PHY2048 CEN3031 CIS4301 CNT4007",
        "insert \"John Doe\" 33333333 1 2 COP3530",
        "insert \"John Doe\" 44444444 1 2 COP3530 CDA3101"
    };

    string expected =
        "unsuccessful\n"
        "unsuccessful\n"
        "unsuccessful\n"
        "successful\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Test 10 hidden style: misspelled commands do not break following commands", "[parser][validation]") {
    vector<string> commands = {
        "insrt \"John Doe\" 11111111 1 1 COP3530",
        "insert \"John Doe\" 11111111 1 1 COP3530",
        "remov 11111111",
        "remove 11111111"
    };

    string expected =
        "unsuccessful\n"
        "successful\n"
        "unsuccessful\n"
        "successful\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Test 11 hidden style: printShortestEdges unreachable after toggles", "[printShortestEdges][graph]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 1 COP3530",
        "printShortestEdges 11111111",
        "toggleEdgesClosure 1 1 2",
        "toggleEdgesClosure 1 1 4",
        "printShortestEdges 11111111"
    };

    string output = runCommands(commands);

    REQUIRE(output.find("successful\n") == 0);
    REQUIRE(output.find("Time For Shortest Edges: John Doe\n") != string::npos);
    REQUIRE(output.find("COP3530: ") != string::npos);
}

TEST_CASE("Test 12 hidden style: remove nonexistent, existing, then double remove invalid", "[remove]") {
    vector<string> commands = {
        "remove 11111111",
        "insert \"John Doe\" 11111111 1 1 COP3530",
        "remove 11111111",
        "remove 11111111"
    };

    string expected =
        "unsuccessful\n"
        "successful\n"
        "successful\n"
        "unsuccessful\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Test 13 hidden style: dropClass edge cases", "[dropClass]") {
    vector<string> commands = {
        "dropClass 11111111 COP3530",
        "insert \"John Doe\" 11111111 1 2 COP3530 CDA3101",
        "dropClass 11111111 MAC2311",
        "dropClass 11111111 COP3530",
        "dropClass 11111111 COP3530",
        "dropClass 11111111 CDA3101",
        "remove 11111111"
    };

    string expected =
        "unsuccessful\n"
        "successful\n"
        "unsuccessful\n"
        "successful\n"
        "unsuccessful\n"
        "successful\n"
        "unsuccessful\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Test 14 hidden style: printStudentZone on 1 class and multiple classes with toggles", "[printStudentZone]") {
    vector<string> commands = {
        "insert \"Solo Student\" 11111111 1 1 COP3530",
        "printStudentZone 11111111",
        "insert \"John Doe\" 88887777 5 4 IDS2935 CEN3031 CIS4301 CNT4007",
        "printStudentZone 88887777",
        "toggleEdgesClosure 2 5 6 9 10",
        "toggleEdgesClosure 2 5 6 9 10",
        "printStudentZone 88887777"
    };

    string output = runCommands(commands);

    REQUIRE(output.find("successful\nStudent Zone Cost For Solo Student: ") != string::npos);
    REQUIRE(output.find("Student Zone Cost For John Doe: 20\n") != string::npos);
}

TEST_CASE("Test 15 hidden style: printShortestEdges after drop and replace", "[printShortestEdges][dropClass][replaceClass]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 3 COP3530 CDA3101 MAC2311",
        "printShortestEdges 11111111",
        "dropClass 11111111 MAC2311",
        "printShortestEdges 11111111",
        "replaceClass 11111111 CDA3101 PHY2048",
        "printShortestEdges 11111111"
    };

    string output = runCommands(commands);

    REQUIRE(output.find("Time For Shortest Edges: John Doe\n") != string::npos);
    REQUIRE(output.find("COP3530: ") != string::npos);
}

TEST_CASE("Additional: removeClass repeated removal", "[removeClass]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 2 COP3530 CDA3101",
        "insert \"Jane Doe\" 22222222 1 1 COP3530",
        "removeClass COP3530",
        "removeClass COP3530",
        "removeClass CDA3101"
    };

    string expected =
        "successful\n"
        "successful\n"
        "2\n"
        "unsuccessful\n"
        "1\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Additional: removeClass removes student with only class", "[removeClass]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 1 COP3530",
        "removeClass COP3530",
        "remove 11111111"
    };

    string expected =
        "successful\n"
        "1\n"
        "unsuccessful\n";

    REQUIRE(runCommands(commands) == expected);
}

TEST_CASE("Additional: invalid extra token parsing", "[parser]") {
    vector<string> commands = {
        "insert \"John Doe\" 11111111 1 1 COP3530",
        "removeClass COP3530 extra",
        "remove 11111111 extra",
        "dropClass 11111111 COP3530 extra",
        "replaceClass 11111111 COP3530 CDA3101 extra"
    };

    string expected =
        "successful\n"
        "unsuccessful\n"
        "unsuccessful\n"
        "unsuccessful\n"
        "unsuccessful\n";

    REQUIRE(runCommands(commands) == expected);
}