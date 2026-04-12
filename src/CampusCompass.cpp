#include "CampusCompass.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <string>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize your object
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    // return boolean based on whether parsing was successful or not
    ifstream edgesfile(edges_filepath);
    ifstream classesfile(classes_filepath);

    if (!edgesfile.is_open() || !classesfile.is_open()) {
        return false;
    }

    string line;

    getline(edgesfile, line);

    while (getline(edgesfile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string id1_str, id2_str, name_1, name_2, time_str;
        getline(ss, id1_str, ',');
        getline(ss, id2_str, ',');
        getline(ss, name_1, ',');
        getline(ss, name_2, ',');
        getline(ss, time_str, ',');

        int id1_num = stoi(id1_str);
        int id2_num = stoi(id2_str);
        int time_num = stoi(time_str);

        location_names[id1_num] = name_1;
        location_names[id2_num] = name_2;

        graph[id1_num].push_back({id2_num, time_num,false});
        graph[id2_num].push_back({id1_num, time_num,false});
    }

    getline(classesfile, line);
    while (getline(classesfile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string class_code, location_idstr, start_time, end_time;

        getline(ss, class_code, ',');
        getline(ss, location_idstr, ',');
        getline(ss, start_time, ',');
        getline(ss, end_time, ',');

        Class_Info info;
        info.location_ID = stoi(location_idstr);
        info.start_Time = start_time;
        info.end_Time = end_time;

        classes[class_code] = info;
    }

    edgesfile.close();
    classesfile.close();

    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    // do whatever regex you need to parse validity
    // hint: return a boolean for validation when testing. For example:
    bool is_valid = true; // replace with your actual validity checking

    return is_valid;
}

void CampusCompass::DebugPrint() {
    cout << "Graph nodes: " << graph.size() << endl;
    cout << "Classes loaded: " << classes.size() << endl;
}
