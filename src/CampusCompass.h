#pragma once
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class CampusCompass {
private:
    struct Edge {
        int to;
        int weight;
        bool closed = false;
    };

    struct Class_Info {
        int location_ID;
        string start_Time;
        string end_Time;
    };

    // adj list graph
    unordered_map<int, vector<Edge>> graph;

    // mad location id -> name
    unordered_map<int, string> location_names;

    // class code -> info
    unordered_map<string, Class_Info> classes;

    // Think about what member variables you need to initialize
    // perhaps some graph representation?
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);

    void DebugPrint();
};
