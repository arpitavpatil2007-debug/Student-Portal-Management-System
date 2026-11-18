#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
using namespace std;

class Person {
protected:
    string name;
public:
    Person(const string& n = "") : name(n) {}
    virtual void display() const = 0;
    virtual ~Person() = default;
};

class Student : public Person {
private:
    string id, course;
    double marks;
public:
    Student() : Person(""), id(""), course(""), marks(0) {}
    Student(string i, string n, string c, double m)
        : Person(n), id(i), course(c), marks(m) {}

    string getId() const { return id; }
    string getName() const { return name; }
    string getCourse() const { return course; }
    double getMarks() const { return marks; }

    void setName(const string& n) { name = n; }
    void setCourse(const string& c) { course = c; }
    void setMarks(double m) {
        if (m < 0 || m > 100) throw invalid_argument("Marks must be between 0 and 100.");
        marks = m;
    }

    string grade() const {
        if (marks >= 90) return "A+";
        if (marks >= 80) return "A";
        if (marks >= 70) return "B";
        if (marks >= 60) return "C";
        if (marks >= 50) return "D";
        return "F";
    }

    void display() const override {
        cout << left << setw(15) << id << setw(24) << name
             << setw(20) << course << right << setw(8)
             << fixed << setprecision(2) << marks << setw(8) << grade() << '\n';
    }
};

class StudentPortal {
private:
    vector<Student> students;
    const string filename = "students.txt";

    string readText(const string& prompt) {
        string value;
        cout << prompt;
        getline(cin, value);
        if (value.empty() || value.find('|') != string::npos)
            throw invalid_argument("Enter a non-empty value without the | character.");
        return value;
    }

    double readMarks() {
        string line;
        cout << "Enter marks (0-100): ";
        getline(cin, line);
        stringstream ss(line);
        double value;
        char extra;
        if (!(ss >> value) || (ss >> extra) || value < 0 || value > 100)
            throw invalid_argument("Enter a valid number between 0 and 100.");
        return value;
    }

    auto findById(const string& id) {
        return find_if(students.begin(), students.end(),
            [&](const Student& s) { return s.getId() == id; });
    }

    void printHeader() const {
        cout << left << setw(15) << "Student ID" << setw(24) << "Name"
             << setw(20) << "Course" << right << setw(8) << "Marks"
             << setw(8) << "Grade" << '\n';
        cout << string(75, '-') << '\n';
    }

public:
    StudentPortal() { loadFromFile(); }

    void loadFromFile() {
        ifstream file(filename);
        if (!file) return; // File is created when the first record is saved.
        string line;
        while (getline(file, line)) {
            string id, n, c, mText;
            stringstream ss(line);
            if (getline(ss, id, '|') && getline(ss, n, '|') &&
                getline(ss, c, '|') && getline(ss, mText)) {
                try {
                    double m = stod(mText);
                    if (!id.empty() && !n.empty() && !c.empty() && m >= 0 && m <= 100 &&
                        findById(id) == students.end())
                        students.emplace_back(id, n, c, m);
                } catch (...) {
                    // Skip malformed records.
                }
            }
        }
    }

    void saveToFile() const {
        ofstream file(filename);
        if (!file) throw runtime_error("Cannot create students.txt in the current folder.");
        for (const Student& s : students)
            file << s.getId() << '|' << s.getName() << '|'
                 << s.getCourse() << '|' << s.getMarks() << '\n';
        if (!file) throw runtime_error("Could not write student records.");
        cout << "Records saved successfully.\n";
    }

    void addStudent() {
        string id = readText("Enter student ID: ");
        if (findById(id) != students.end())
            throw invalid_argument("This student ID already exists.");
        string n = readText("Enter student name: ");
        string c = readText("Enter course: ");
        double m = readMarks();
        students.emplace_back(id, n, c, m);
        saveToFile();
        cout << "Student added successfully.\n";
    }

    void displayAll() const {
        if (students.empty()) { cout << "No student records found.\n"; return; }
        printHeader();
        for (const Student& s : students) s.display();
    }

    void searchStudent() {
        string id = readText("Enter student ID: ");
        auto it = findById(id);
        if (it == students.end()) cout << "Student not found.\n";
        else { printHeader(); it->display(); }
    }

    void updateStudent() {
        string id = readText("Enter student ID to update: ");
        auto it = findById(id);
        if (it == students.end()) { cout << "Student not found.\n"; return; }
        cout << "1. Update name\n2. Update course\n3. Update marks\nChoose: ";
        string choice;
        getline(cin, choice);
        if (choice == "1") it->setName(readText("Enter new name: "));
        else if (choice == "2") it->setCourse(readText("Enter new course: "));
        else if (choice == "3") it->setMarks(readMarks());
        else { cout << "Invalid choice.\n"; return; }
        saveToFile();
        cout << "Student details updated.\n";
    }

    void deleteStudent() {
        string id = readText("Enter student ID to delete: ");
        auto it = findById(id);
        if (it == students.end()) { cout << "Student not found.\n"; return; }
        students.erase(it);
        saveToFile();
        cout << "Student record deleted.\n";
    }

    void updateMarks() {
        string id = readText("Enter student ID: ");
        auto it = findById(id);
        if (it == students.end()) { cout << "Student not found.\n"; return; }
        it->setMarks(readMarks());
        saveToFile();
        cout << "Marks updated. Grade: " << it->grade() << '\n';
    }

    void showResult() {
        string id = readText("Enter student ID: ");
        auto it = findById(id);
        if (it == students.end()) { cout << "Student not found.\n"; return; }
        cout << "Name       : " << it->getName() << '\n'
             << "Student ID : " << it->getId() << '\n'
             << "Course     : " << it->getCourse() << '\n'
             << "Marks      : " << fixed << setprecision(2) << it->getMarks() << "/100\n"
             << "Percentage : " << it->getMarks() << "%\n"
             << "Grade      : " << it->grade() << '\n';
    }

    void run() {
        cout << "==========================================\n"
             << "      STUDENT PORTAL MANAGEMENT SYSTEM\n"
             << "         Developed by Arpita Patil\n"
             << "             E25B000429\n"
             << "==========================================\n";
        while (true) {
            cout << "\n1. Add New Student\n2. Display All Students\n"
                 << "3. Search Student\n4. Update Student Details\n"
                 << "5. Delete Student Record\n6. Add / Update Marks\n"
                 << "7. Calculate Result and Grade\n8. Save Student Records\n"
                 << "9. Exit\nChoose an option: ";
            string choice;
            if (!getline(cin, choice)) break;
            try {
                if (choice == "1") addStudent();
                else if (choice == "2") displayAll();
                else if (choice == "3") searchStudent();
                else if (choice == "4") updateStudent();
                else if (choice == "5") deleteStudent();
                else if (choice == "6") updateMarks();
                else if (choice == "7") showResult();
                else if (choice == "8") saveToFile();
                else if (choice == "9") { saveToFile(); cout << "Thank you!\n"; break; }
                else cout << "Please choose a number from 1 to 9.\n";
            } catch (const exception& e) {
                cout << "Error: " << e.what() << '\n';
            }
        }
    }
};

int main() {
    try {
        StudentPortal portal;
        portal.run();
    } catch (const exception& e) {
        cerr << "Application error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
