#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <iomanip>

using namespace std;

#define N 3

void showArray(int arr[N][N]) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << setw(8) << arr[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

int main() {

    pid_t pids[N];
    int pd[N][2];


    int a[N][N];
    int b[N][N];


    for (int i = 0, k = 1; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            a[i][j] = k++;
            b[i][j] = k++;
        }
    }
    cout << "A:" << endl;
    showArray(a);
    cout << "B:" << endl;
    showArray(b);


    for (int i = 0; i < N; ++i) {

        if (pipe(pd[i]) == -1) {
            fprintf(stderr, "Pipe %d failed!\n", i);
            return 1;
        }

        pid_t pid = fork();
        if (pid == 0) {
            int ri[N];
            for (int j = 0; j < N; ++j) {
                ri[j] = 0;
                for (int k = 0; k < N; ++k) {
                    ri[j] += a[i][k] * b[k][j];
                }
            }

            write(pd[i][1], ri, N * sizeof(int));
            close(pd[i][0]);
            close(pd[i][1]);

            exit(0);
        }
    }


    int c[N][N];

    for (int i = 0; i < N; ++i) {

        waitpid(pids[i], nullptr, 0);
        read(pd[i][0], c[i], N * sizeof(int));

        close(pd[i][0]);
        close(pd[i][1]);
    }

    cout << "C:" << endl;
    showArray(c);

    return 0;
}