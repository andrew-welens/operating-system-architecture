#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    ifstream file("coordinates.txt");
    string line;
    while (getline(file, line)) {
        float lat = stof(line.substr(line.find('(') + 1, line.find(',') - line.find('(') - 1));
        float lon = stof(line.substr(line.find(',') + 2, line.find(')') - line.find(',') - 2));
        if (lat >= 50 && lat <= 80 && lon >= 20 && lon <= 45) {
            cout << line << endl;
        }
    }
    return 0;
}