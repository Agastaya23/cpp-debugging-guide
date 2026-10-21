#include <iostream>
#include <vector>

// Recursive binary search.
// Returns the position of target in the sorted vector v, or -1 if it is not there.
// Only looks between positions lo and hi (inclusive).
// FIXED: skip past mid (mid + 1) so the range always gets smaller.
int search(const std::vector<int>& v, int target, int lo, int hi) {
    if (lo > hi) {
        return -1;
    }
    int mid = (lo + hi) / 2;
    if (v[mid] == target) {
        return mid;
    }
    if (v[mid] < target) {
        return search(v, target, mid + 1, hi);
    }
    return search(v, target, lo, mid - 1);
}

int find(const std::vector<int>& v, int target) {
    return search(v, target, 0, static_cast<int>(v.size()) - 1);
}

int main() {
    std::vector<int> numbers = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};

    std::cout << "16 is at position " << find(numbers, 16) << std::endl;  // 4
    std::cout << "2 is at position " << find(numbers, 2) << std::endl;    // 0
    std::cout << "91 is at position " << find(numbers, 91) << std::endl;  // 9
    return 0;
}
