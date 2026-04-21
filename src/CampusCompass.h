#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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

    struct Student {
        string name;
        string id;
        int residence_ID;
        vector<string> class_codes;
    };

    unordered_map<int, vector<Edge>> graph;
    unordered_map<int, string> location_names;
    unordered_map<string, Class_Info> classes;
    unordered_map<string, Student> students;

    // validation helpers
    bool IsValidUFID(const string& id) const;
    bool IsValidName(const string& name) const;
    bool IsValidClassCode(const string& code) const;
    bool StudentHasClass(const Student& s, const string& code) const;

    // graph helpers
    bool EdgeExists(int u, int v) const;
    bool IsEdgeClosed(int u, int v) const;
    void ToggleEdge(int u, int v);
    bool IsReachable(int start, int goal) const;
    int ShortestPathCost(int start, int goal) const;

    // command helpers
    bool InsertStudent(const string& name, const string& id, int residence_ID,
                       const vector<string>& class_codes);
    bool RemoveStudent(const string& id);
    bool DropClass(const string& id, const string& class_code);
    bool ReplaceClass(const string& id, const string& old_code, const string& new_code);
    int RemoveClassFromAll(const string& class_code);

public:
    CampusCompass();

    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);

    void DebugPrint() const;
};