#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

using namespace std;

#define ERROR_CREATE_THREAD -11
#define ERROR_JOIN_THREAD -12
#define SUCCESS 0

// A[L,M] * B[M,N] = C[L,N]
#define L 2
#define M 2
#define N 2

typedef struct args_tag {
    int i;
    int (*a)[L][M];
    int (*b)[M][N];
    int (*r)[N];
} args_t;


void *calculateRow(void *args) {
    args_t *arg = (args_t *)args;
    for (int i = 0; i < N; i++) {
        (*arg->r)[i] = 0;
        for (int j = 0; j < M; j++) {
            (*arg->r)[i] += (*arg->a)[arg->i][j] * (*arg->b)[j][i];
        }
    }

    return SUCCESS;
}

int main() {
    int A[L][M];
    int B[M][N];
    int C[L][N];

    // init arrays
    int v = 0;
    for (int j = 0; j < M; j++) {
        for (int i = 0; i < L; i++) {
            v++;
            A[i][j] = v;
        }
        for (int k = 0; k < N; k++) {
            v++;
            B[j][k] = v;
        }
    }

    // thread for each row
    pthread_t threads[L];
    args_t args[L];
    int status;
    long status_addr;

    // init array of arguments
    for (int i = 0; i < L; i++) {
        args[i].i = i;
        args[i].a = &A;
        args[i].b = &B;
        args[i].r = &C[i];
    }

    // run thread for each row
    for (int i = 0; i < L; i++) {
        status = pthread_create(&threads[i], NULL, calculateRow, (void *)&args[i]);
        if (status != 0) {
            printf("main error: can't create thread, status = %ld\n", status);
            exit(ERROR_CREATE_THREAD);
        }
    }
    // wait threads
    for (int i = 0; i < L; i++) {
        status = pthread_join(threads[i], (void **)&status_addr);
        if (status != SUCCESS) {
            printf("main error: can't join thread, status = %d\n", status);
            exit(ERROR_JOIN_THREAD);
        }
    }

    // print A
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < M; ++j) {
            cout << setw(8) << A[i][j];
        }
        cout << endl;
    }
    cout << endl;

    // print B
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << setw(8) << B[i][j];
        }
        cout << endl;
    }
    cout << endl;

    // print C
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << setw(8) << C[i][j];
        }
        cout << endl;
    }
    cout << endl;


    return 0;
}