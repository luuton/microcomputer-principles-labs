#include <stdio.h>

int PENO(int *X, int N, int *SP, int *SN) {
    *SP = 0;    // 正奇数和
    *SN = 0;    // 负偶数和
    for (int i = 0; i < N; i++) {
        if (X[i] > 0 && X[i] % 2 == 1) {
            *SP += X[i];
        } else if (X[i] < 0 && X[i] % 2 == 0) {
            *SN += X[i];
        }
    }
    return 0;
}

int main(void) {
    int X1[10]={1,-4,8,-9,5,6,-10,19,22,23};
    int SP, SN;
    PENO(X1, 10, &SP, &SN);
    printf("SP = %d, SN = %d\n", SP, SN);

    int X2[10]={121,-124,138,-199,255,2566,-1034,1019,2032,2033};
    PENO(X2, 10, &SP, &SN);
    printf("SP = %d, SN = %d\n", SP, SN);

    return 0;

}