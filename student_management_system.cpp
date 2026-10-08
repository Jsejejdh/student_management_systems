// Student Management System (console, C++)
// Features: add, display, search, update, delete students with file persistence.
// Data is stored in "students.txt" (one record per line, fields separated by '|').

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const string FILE_NAME = "students.txt";

struct Student {
    int roll;
    string name;
    int age;
    string course;
    double marks;
};


string readLine(const string& prompt, bool allowEmpty = false) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        // trim
        size_t a = s.find_first_not_of(" \t\r\n");
        size_t b = s.find_last_not_of(" \t\r\n");
        s = (a == string::npos) ? "" : s.substr(a, b - a + 1);
        // '|' is our file delimiter, so don't allow it in the data
        replace(s.begin(), s.end(), '|', '/');
        if (!s.empty() || allowEmpty) return s;
        cout << "  Input cannot be empty. Try again.\n";
    }
}

bool parseInt(const string& s, int& out) {
    try {
        size_t pos;
        out = stoi(s, &pos);
        return pos == s.size();
    } catch (...) {
        return false;
    }
}

bool parseDouble(const string& s, double& out) {
    try {
        size_t pos;
        out = stod(s, &pos);
        return pos == s.size();
    } catch (...) {
        return false;
    }
}

int readInt(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        string s = readLine(prompt);
        if (parseInt(s, v) && v >= lo && v <= hi) return v;
        cout << "  Please enter a whole number between " << lo << " and " << hi << ".\n";
    }
}

double readMarks(const string& prompt) {
    double v;
    while (true) {
        string s = readLine(prompt);
        if (parseDouble(s, v) && v >= 0 && v <= 100) return v;
        cout << "  Please enter marks between 0 and 100.\n";
    }
}

// ---------- File handling ----------

vector<Student> loadStudents() {
    vector<Student> list;
    ifstream in(FILE_NAME);
    if (!in) return list;  // no file yet: start empty

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string roll, name, age, course, marks;
        getline(ss, roll, '|');
        getline(ss, name, '|');
        getline(ss, age, '|');
        getline(ss, course, '|');
        getline(ss, marks, '|');

        Student s;
        if (parseInt(roll, s.roll) && parseInt(age, s.age) && parseDouble(marks, s.marks)) {
            s.name = name;
            s.course = course;
            list.push_back(s);
        }
    }
    return list;
}

bool saveStudents(const vector<Student>& list) {
    ofstream out(FILE_NAME, ios::trunc);
    if (!out) {
        cout << "  Error: could not write to " << FILE_NAME << "\n";
        return false;
    }
    for (const auto& s : list)
        out << s.roll << '|' << s.name << '|' << s.age << '|' << s.course << '|' << s.marks << '\n';
    return true;
}

// ---------- Helpers ----------

int findIndex(const vector<Student>& list, int roll) {
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i].roll == roll) return static_cast<int>(i);
    return -1;
}

void printHeader() {
    cout << left << setw(8) << "Roll" << setw(24) << "Name" << setw(6) << "Age"
         << setw(18) << "Course" << setw(8) << "Marks" << "\n";
    cout << string(64, '-') << "\n";
}

void printStudent(const Student& s) {
    cout << left << setw(8) << s.roll << setw(24) << s.name << setw(6) << s.age
         << setw(18) << s.course << fixed << setprecision(2) << s.marks << "\n";
}

// ---------- Operations ----------

void addStudent(vector<Student>& list) {
    cout << "\n--- Add Student ---\n";
    Student s;
    s.roll = readInt("Roll number: ", 1, 999999);
    if (findIndex(list, s.roll) != -1) {
        cout << "  A student with roll number " << s.roll << " already exists.\n";
        return;
    }
    s.name = readLine("Name: ");
    s.age = readInt("Age: ", 3, 100);
    s.course = readLine("Course: ");
    s.marks = readMarks("Marks (0-100): ");

    list.push_back(s);
    if (saveStudents(list)) cout << "  Student added successfully.\n";
}

void displayStudents(const vector<Student>& list) {
    cout << "\n--- All Students ---\n";
    if (list.empty()) {
        cout << "  No records found.\n";
        return;
    }
    printHeader();
    for (const auto& s : list) printStudent(s);
    cout << "\nTotal students: " << list.size() << "\n";
}

void searchStudent(const vector<Student>& list) {
    cout << "\n--- Search Student ---\n";
    int roll = readInt("Enter roll number: ", 1, 999999);
    int idx = findIndex(list, roll);
    if (idx == -1) {
        cout << "  No student found with roll number " << roll << ".\n";
        return;
    }
    printHeader();
    printStudent(list[idx]);
}

void updateStudent(vector<Student>& list) {
    cout << "\n--- Update Student ---\n";
    int roll = readInt("Enter roll number to update: ", 1, 999999);
    int idx = findIndex(list, roll);
    if (idx == -1) {
        cout << "  No student found with roll number " << roll << ".\n";
        return;
    }

    Student& s = list[idx];
    cout << "Current record:\n";
    printHeader();
    printStudent(s);
    cout << "\nPress Enter to keep the current value.\n";

    string in = readLine("New name [" + s.name + "]: ", true);
    if (!in.empty()) s.name = in;

    while (true) {
        in = readLine("New age [" + to_string(s.age) + "]: ", true);
        if (in.empty()) break;
        int a;
        if (parseInt(in, a) && a >= 3 && a <= 100) { s.age = a; break; }
        cout << "  Please enter a whole number between 3 and 100.\n";
    }

    in = readLine("New course [" + s.course + "]: ", true);
    if (!in.empty()) s.course = in;

    while (true) {
        in = readLine("New marks [" + to_string(static_cast<int>(s.marks)) + "]: ", true);
        if (in.empty()) break;
        double m;
        if (parseDouble(in, m) && m >= 0 && m <= 100) { s.marks = m; break; }
        cout << "  Please enter marks between 0 and 100.\n";
    }

    if (saveStudents(list)) cout << "  Record updated successfully.\n";
}

void deleteStudent(vector<Student>& list) {
    cout << "\n--- Delete Student ---\n";
    int roll = readInt("Enter roll number to delete: ", 1, 999999);
    int idx = findIndex(list, roll);
    if (idx == -1) {
        cout << "  No student found with roll number " << roll << ".\n";
        return;
    }
    printHeader();
    printStudent(list[idx]);
    string confirm = readLine("Delete this record? (y/n): ");
    if (confirm == "y" || confirm == "Y") {
        list.erase(list.begin() + idx);
        if (saveStudents(list)) cout << "  Record deleted.\n";
    } else {
        cout << "  Deletion cancelled.\n";
    }
}

// ---------- Main menu ----------

void showMenu() {
    cout << "\n=============================\n"
         << "   STUDENT MANAGEMENT SYSTEM\n"
         << "=============================\n"
         << " 1. Add student\n"
         << " 2. Display all students\n"
         << " 3. Search student\n"
         << " 4. Update student\n"
         << " 5. Delete student\n"
         << " 6. Exit\n"
         << "-----------------------------\n";
}

int main() {
    vector<Student> students = loadStudents();
    cout << "Loaded " << students.size() << " record(s) from " << FILE_NAME << ".\n";

    while (true) {
        showMenu();
        int choice = readInt("Enter your choice (1-6): ", 1, 6);
        switch (choice) {
            case 1: addStudent(students); break;
            case 2: displayStudents(students); break;
            case 3: searchStudent(students); break;
            case 4: updateStudent(students); break;
            case 5: deleteStudent(students); break;
            case 6:
                cout << "Goodbye!\n";
                return 0;
        }
    }
}