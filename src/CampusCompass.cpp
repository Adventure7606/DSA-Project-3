#include "CampusCompass.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <queue>
#include <climits>
#include <algorithm>
#include <functional>

using namespace std;

CampusCompass::CampusCompass() {}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    graph.clear();
    location_names.clear();
    classes.clear();
    students.clear();

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

        int id1 = stoi(id1_str);
        int id2 = stoi(id2_str);
        int weight = stoi(time_str);

        location_names[id1] = name_1;
        location_names[id2] = name_2;

        graph[id1].push_back({id2, weight, false});
        graph[id2].push_back({id1, weight, false});
    }

    getline(classesfile, line);
    while (getline(classesfile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string class_code, location_id_str, start_time, end_time;

        getline(ss, class_code, ',');
        getline(ss, location_id_str, ',');
        getline(ss, start_time, ',');
        getline(ss, end_time, ',');

        Class_Info info;
        info.location_ID = stoi(location_id_str);
        info.start_Time = start_time;
        info.end_Time = end_time;

        classes[class_code] = info;
    }

    edgesfile.close();
    classesfile.close();
    return true;
}

bool CampusCompass::IsValidUFID(const string& id) const {
    static const regex pattern("^[0-9]{8}$");
    return regex_match(id, pattern);
}

bool CampusCompass::IsValidName(const string& name) const {
    static const regex pattern("^[A-Za-z ]+$");
    return !name.empty() && regex_match(name, pattern);
}

bool CampusCompass::IsValidClassCode(const string& code) const {
    static const regex pattern("^[A-Z]{3}[0-9]{4}$");
    return regex_match(code, pattern);
}

bool CampusCompass::StudentHasClass(const Student& s, const string& code) const {
    for (const string& c : s.class_codes) {
        if (c == code) return true;
    }
    return false;
}

bool CampusCompass::EdgeExists(int u, int v) const {
    auto it = graph.find(u);
    if (it == graph.end()) return false;

    for (const Edge& e : it->second) {
        if (e.to == v) return true;
    }
    return false;
}

bool CampusCompass::IsEdgeClosed(int u, int v) const {
    auto it = graph.find(u);
    if (it == graph.end()) return false;

    for (const Edge& e : it->second) {
        if (e.to == v) return e.closed;
    }
    return false;
}

void CampusCompass::ToggleEdge(int u, int v) {
    for (Edge& e : graph[u]) {
        if (e.to == v) {
            e.closed = !e.closed;
            break;
        }
    }

    for (Edge& e : graph[v]) {
        if (e.to == u) {
            e.closed = !e.closed;
            break;
        }
    }
}

bool CampusCompass::IsReachable(int start, int goal) const {
    if (graph.find(start) == graph.end() || graph.find(goal) == graph.end()) {
        return false;
    }

    unordered_set<int> visited;
    queue<int> q;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        int curr = q.front();
        q.pop();

        if (curr == goal) return true;

        auto it = graph.find(curr);
        if (it == graph.end()) continue;

        for (const Edge& e : it->second) {
            if (e.closed) continue;
            if (!visited.count(e.to)) {
                visited.insert(e.to);
                q.push(e.to);
            }
        }
    }

    return false;
}

int CampusCompass::ShortestPathCost(int start, int goal) const {
    if (graph.find(start) == graph.end() || graph.find(goal) == graph.end()) {
        return -1;
    }

    unordered_map<int, int> dist;
    for (const auto& pair : graph) {
        dist[pair.first] = INT_MAX;
    }

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currDist, u] = pq.top();
        pq.pop();

        if (currDist > dist[u]) continue;
        if (u == goal) return currDist;

        for (const Edge& e : graph.at(u)) {
            if (e.closed) continue;

            int v = e.to;
            int newDist = currDist + e.weight;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                pq.push({newDist, v});
            }
        }
    }

    return -1;
}

vector<int> CampusCompass::ShortestPathNodes(int start, int goal) const {
    if (graph.find(start) == graph.end() || graph.find(goal) == graph.end()) {
        return {};
    }

    unordered_map<int, int> dist;
    unordered_map<int, int> parent;

    for (const auto& pair : graph) {
        dist[pair.first] = INT_MAX;
    }

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    dist[start] = 0;
    parent[start] = -1;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [currDist, u] = pq.top();
        pq.pop();

        if (currDist > dist[u]) continue;

        for (const Edge& e : graph.at(u)) {
            if (e.closed) continue;

            int v = e.to;
            int newDist = currDist + e.weight;

            if (newDist < dist[v]) {
                dist[v] = newDist;
                parent[v] = u;
                pq.push({newDist, v});
            }
        }
    }

    if (dist[goal] == INT_MAX) {
        return {};
    }

    vector<int> path;
    int curr = goal;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }

    reverse(path.begin(), path.end());
    return path;
}

int CampusCompass::PrintStudentZoneCost(const string& id) const {
    auto it = students.find(id);
    if (it == students.end()) return -1;

    const Student& s = it->second;
    unordered_set<int> zone_nodes;

    for (const string& code : s.class_codes) {
        auto class_it = classes.find(code);
        if (class_it == classes.end()) return -1;

        int class_location = class_it->second.location_ID;
        vector<int> path = ShortestPathNodes(s.residence_ID, class_location);

        if (path.empty()) {
            return -1;
        }

        for (int node : path) {
            zone_nodes.insert(node);
        }
    }

    struct MSTEdge {
        int u;
        int v;
        int w;
    };

    vector<MSTEdge> subgraph_edges;
    unordered_set<string> seen_edges;

    for (const auto& pair : graph) {
        int u = pair.first;
        if (!zone_nodes.count(u)) continue;

        for (const Edge& e : pair.second) {
            int v = e.to;
            if (e.closed) continue;
            if (!zone_nodes.count(v)) continue;

            int a = min(u, v);
            int b = max(u, v);
            string key = to_string(a) + "#" + to_string(b);

            if (!seen_edges.count(key)) {
                seen_edges.insert(key);
                subgraph_edges.push_back({a, b, e.weight});
            }
        }
    }

    sort(subgraph_edges.begin(), subgraph_edges.end(), [](const MSTEdge& lhs, const MSTEdge& rhs) {
        return lhs.w < rhs.w;
    });

    unordered_map<int, int> parent;
    unordered_map<int, int> rank_map;

    for (int node : zone_nodes) {
        parent[node] = node;
        rank_map[node] = 0;
    }

    function<int(int)> find_set = [&](int x) {
        if (parent[x] == x) return x;
        parent[x] = find_set(parent[x]);
        return parent[x];
    };

    auto union_set = [&](int a, int b) {
        a = find_set(a);
        b = find_set(b);

        if (a == b) return false;

        if (rank_map[a] < rank_map[b]) {
            swap(a, b);
        }

        parent[b] = a;
        if (rank_map[a] == rank_map[b]) {
            rank_map[a]++;
        }

        return true;
    };

    int mst_cost = 0;
    int edges_used = 0;
    int needed = (int)zone_nodes.size() - 1;

    for (const MSTEdge& e : subgraph_edges) {
        if (union_set(e.u, e.v)) {
            mst_cost += e.w;
            edges_used++;
            if (edges_used == needed) break;
        }
    }

    if (edges_used != needed) return -1;
    return mst_cost;
}

int CampusCompass::TimeToMinutes(const string& time_str) const {
    size_t colon = time_str.find(':');
    if (colon == string::npos) return -1;

    int hour = stoi(time_str.substr(0, colon));
    int minute = stoi(time_str.substr(colon + 1));

    if (hour >= 1 && hour <= 7) {
        hour += 12;
    }

    return hour * 60 + minute;
}

bool CampusCompass::VerifySchedule(const string& id) const {
    auto it = students.find(id);
    if (it == students.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    const Student& s = it->second;

    if (s.class_codes.size() <= 1) {
        cout << "unsuccessful" << endl;
        return false;
    }

    struct ScheduledClass {
        string code;
        int location;
        int start_min;
        int end_min;
    };

    vector<ScheduledClass> ordered;

    for (const string& code : s.class_codes) {
        auto class_it = classes.find(code);
        if (class_it == classes.end()) {
            cout << "unsuccessful" << endl;
            return false;
        }

        ScheduledClass sc;
        sc.code = code;
        sc.location = class_it->second.location_ID;
        sc.start_min = TimeToMinutes(class_it->second.start_Time);
        sc.end_min = TimeToMinutes(class_it->second.end_Time);
        ordered.push_back(sc);
    }

    sort(ordered.begin(), ordered.end(), [](const ScheduledClass& a, const ScheduledClass& b) {
        if (a.start_min != b.start_min) return a.start_min < b.start_min;
        return a.code < b.code;
    });

    cout << "Schedule Check for " << s.name << ":" << endl;

    for (size_t i = 0; i + 1 < ordered.size(); i++) {
        const ScheduledClass& curr = ordered[i];
        const ScheduledClass& next = ordered[i + 1];

        int gap = next.start_min - curr.end_min;
        int travel = ShortestPathCost(curr.location, next.location);

        bool ok = (travel != -1 && gap >= travel);

        cout << curr.code << " - " << next.code << ": "
             << (ok ? "successful" : "unsuccessful") << endl;
    }

    return true;
}

bool CampusCompass::InsertStudent(const string& name, const string& id, int residence_ID,const vector<string>& class_codes) {
    if (!IsValidName(name)) return false;
    if (!IsValidUFID(id)) return false;
    if (students.find(id) != students.end()) return false;
    if (graph.find(residence_ID) == graph.end()) return false;
    if (class_codes.size() < 1 || class_codes.size() > 6) return false;

    unordered_set<string> seen;
    for (const string& code : class_codes) {
        if (!IsValidClassCode(code)) return false;
        if (classes.find(code) == classes.end()) return false;
        if (seen.count(code)) return false;
        seen.insert(code);
    }

    Student s;
    s.name = name;
    s.id = id;
    s.residence_ID = residence_ID;
    s.class_codes = class_codes;

    students[id] = s;
    return true;
}

bool CampusCompass::RemoveStudent(const string& id) {
    auto it = students.find(id);
    if (it == students.end()) return false;

    students.erase(it);
    return true;
}

bool CampusCompass::DropClass(const string& id, const string& class_code) {
    auto it = students.find(id);
    if (it == students.end()) return false;
    if (!IsValidClassCode(class_code)) return false;
    if (classes.find(class_code) == classes.end()) return false;

    Student& s = it->second;
    auto vec_it = find(s.class_codes.begin(), s.class_codes.end(), class_code);

    if (vec_it == s.class_codes.end()) return false;

    s.class_codes.erase(vec_it);

    if (s.class_codes.empty()) {
        students.erase(id);
    }

    return true;
}

bool CampusCompass::ReplaceClass(const string& id, const string& old_code, const string& new_code) {
    auto it = students.find(id);
    if (it == students.end()) return false;

    if (!IsValidClassCode(old_code) || !IsValidClassCode(new_code)) return false;
    if (classes.find(new_code) == classes.end()) return false;

    Student& s = it->second;
    auto old_it = find(s.class_codes.begin(), s.class_codes.end(), old_code);

    if (old_it == s.class_codes.end()) return false;
    if (find(s.class_codes.begin(), s.class_codes.end(), new_code) != s.class_codes.end()) {
        return false;
    }

    *old_it = new_code;
    return true;
}

int CampusCompass::RemoveClassFromAll(const string& class_code) {
    if (!IsValidClassCode(class_code)) return -1;
    if (classes.find(class_code) == classes.end()) return -1;

    int affected = 0;
    vector<string> to_remove_students;

    for (auto& pair : students) {
        Student& s = pair.second;
        auto it = find(s.class_codes.begin(), s.class_codes.end(), class_code);

        if (it != s.class_codes.end()) {
            s.class_codes.erase(it);
            affected++;

            if (s.class_codes.empty()) {
                to_remove_students.push_back(pair.first);
            }
        }
    }

    for (const string& id : to_remove_students) {
        students.erase(id);
    }

    if (affected == 0) return -1;
    return affected;
}

bool CampusCompass::ParseCommand(const string &command) {
    if (command.empty()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    stringstream ss(command);
    string op;
    ss >> op;

    if (op == "insert") {
        size_t first_quote = command.find('"');
        size_t second_quote = command.find('"', first_quote + 1);

        if (first_quote == string::npos || second_quote == string::npos || second_quote <= first_quote + 1) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string name = command.substr(first_quote + 1, second_quote - first_quote - 1);
        string rest = command.substr(second_quote + 1);
        stringstream rest_ss(rest);

        string id;
        int residence_ID, n;

        if (!(rest_ss >> id >> residence_ID >> n)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        vector<string> class_codes;
        string code;
        while (rest_ss >> code) {
            class_codes.push_back(code);
        }

        if ((int)class_codes.size() != n) {
            cout << "unsuccessful" << endl;
            return false;
        }

        bool ok = InsertStudent(name, id, residence_ID, class_codes);
        cout << (ok ? "successful" : "unsuccessful") << endl;
        return ok;
    }

    else if (op == "remove") {
        string id;
        if (!(ss >> id)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        bool ok = RemoveStudent(id);
        cout << (ok ? "successful" : "unsuccessful") << endl;
        return ok;
    }

    else if (op == "dropClass") {
        string id, code;
        if (!(ss >> id >> code)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        bool ok = DropClass(id, code);
        cout << (ok ? "successful" : "unsuccessful") << endl;
        return ok;
    }

    else if (op == "replaceClass") {
        string id, old_code, new_code;
        if (!(ss >> id >> old_code >> new_code)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        bool ok = ReplaceClass(id, old_code, new_code);
        cout << (ok ? "successful" : "unsuccessful") << endl;
        return ok;
    }

    else if (op == "removeClass") {
        string code;
        if (!(ss >> code)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        int result = RemoveClassFromAll(code);
        if (result == -1) {
            cout << "unsuccessful" << endl;
            return false;
        }

        cout << result << endl;
        return true;
    }

    else if (op == "toggleEdgesClosure") {
        int n;
        if (!(ss >> n) || n < 0) {
            cout << "unsuccessful" << endl;
            return false;
        }

        vector<int> ids;
        int x;
        while (ss >> x) {
            ids.push_back(x);
        }

        if ((int)ids.size() != 2 * n) {
            cout << "unsuccessful" << endl;
            return false;
        }

        for (int i = 0; i < 2 * n; i += 2) {
            int u = ids[i];
            int v = ids[i + 1];
            if (!EdgeExists(u, v)) {
                cout << "unsuccessful" << endl;
                return false;
            }
        }

        for (int i = 0; i < 2 * n; i += 2) {
            ToggleEdge(ids[i], ids[i + 1]);
        }

        cout << "successful" << endl;
        return true;
    }

    else if (op == "checkEdgeStatus") {
        int u, v;
        if (!(ss >> u >> v)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        if (!EdgeExists(u, v)) {
            cout << "DNE" << endl;
        }
        else if (IsEdgeClosed(u, v)) {
            cout << "closed" << endl;
        }
        else {
            cout << "open" << endl;
        }

        return true;
    }

    else if (op == "isConnected") {
        int u, v;
        if (!(ss >> u >> v)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        bool ok = IsReachable(u, v);
        cout << (ok ? "successful" : "unsuccessful") << endl;
        return ok;
    }

    else if (op == "printShortestEdges") {
        string id;
        if (!(ss >> id)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        auto it = students.find(id);
        if (it == students.end()) {
            cout << "unsuccessful" << endl;
            return false;
        }

        const Student& s = it->second;
        vector<string> sorted_codes = s.class_codes;
        sort(sorted_codes.begin(), sorted_codes.end());

        cout << "Time For Shortest Edges: " << s.name << endl;
        for (const string& code : sorted_codes) {
            int destination = classes.at(code).location_ID;
            int cost = ShortestPathCost(s.residence_ID, destination);
            cout << code << ": " << cost << endl;
        }

        return true;
    }

    else if (op == "printStudentZone") {
        string id;
        if (!(ss >> id)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        auto it = students.find(id);
        if (it == students.end()) {
            cout << "unsuccessful" << endl;
            return false;
        }

        int cost = PrintStudentZoneCost(id);
        if (cost == -1) {
            cout << "unsuccessful" << endl;
            return false;
        }

        cout << "Student Zone Cost For " << it->second.name << ": " << cost << endl;
        return true;
    }

    else if (op == "verifySchedule") {
        string id;
        if (!(ss >> id)) {
            cout << "unsuccessful" << endl;
            return false;
        }

        string extra;
        if (ss >> extra) {
            cout << "unsuccessful" << endl;
            return false;
        }

        return VerifySchedule(id);
    }

    else {
        cout << "unsuccessful" << endl;
        return false;
    }
}

void CampusCompass::DebugPrint() const {
    cout << "Graph nodes: " << graph.size() << endl;
    cout << "Classes loaded: " << classes.size() << endl;
    cout << "Students loaded: " << students.size() << endl;
}