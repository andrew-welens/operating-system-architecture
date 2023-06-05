#include <unistd.h>
#include <iostream>

using namespace std;

int main() {
    pid_t child_pid;

    child_pid = fork();
    if (child_pid > 0) {
        cout << child_pid << endl;
        sleep(10);
    } else {
        exit(0);
    }

    return 0;
}