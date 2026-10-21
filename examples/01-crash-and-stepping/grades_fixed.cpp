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

// FIXED: convert to double before dividing, so the decimals are kept.
double averageOf(const std::vector<int>& scores) {
    int total = totalOf(scores);
    int count = scores.size();
    double average = static_cast<double>(total) / count;
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

// FIXED: check for nullptr before using the pointer.
void printReport(std::vector<Student>& students, const std::string& name) {
    Student* student = findStudent(students, name);
    if (student == nullptr) {
        std::cout << name << ": not found" << std::endl;
        return;
    }
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
