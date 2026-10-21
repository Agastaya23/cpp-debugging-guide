#include <iostream>
#include <string>
#include <vector>

struct Student {
    std::string name;
    std::vector<int> scores;
};

// Adds up all the scores.
int totalOf(const std::vector<int>& scores) {
    int total = 0;
    for (int score : scores) {
        total += score;
    }
    return total;
}

// BUG #1 is hiding in here: the averages come out slightly wrong.
double averageOf(const std::vector<int>& scores) {
    int total = totalOf(scores);
    int count = scores.size();
    double average = total / count;
    return average;
}

// Returns the student with this name, or nullptr if nobody has it.
Student* findStudent(std::vector<Student>& students, const std::string& name) {
    for (Student& s : students) {
        if (s.name == name) {
            return &s;
        }
    }
    return nullptr;
}

// BUG #2: the program crashes for one of the names.
void printReport(std::vector<Student>& students, const std::string& name) {
    Student* student = findStudent(students, name);
    double avg = averageOf(student->scores);
    std::cout << student->name << ": " << avg << std::endl;
}

int main() {
    std::vector<Student> students = {
        {"Alice", {90, 85, 80, 78}},
        {"Bob",   {70, 65, 88}},
    };

    printReport(students, "Alice");    // should print 83.25
    printReport(students, "Bob");      // should print 74.3333
    printReport(students, "Charlie");  // should not crash
    return 0;
}
