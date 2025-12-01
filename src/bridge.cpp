#include <iostream>
#include <fstream>
#include <vector>

int main() {
    std::ifstream file("data.txt");
    if (!file) {
        std::cerr << "Error: Could not open file.\n";
        return 1;
    }

    std::vector<int> values;
    int num;
    while (file >> num) {
        values.push_back(num);
    }

    std::cout << "Received values:";
    for (int v : values) std::cout << " " << v;
    std::cout << "\n";
    return 0;
}