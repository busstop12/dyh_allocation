#include <stdio.h>

#include "dyh.h"

int main(int argc, char const *argv[]) {
    int n;
    scanf("%d", &n);
    int *num = (int *)dyh_malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        scanf("%d", num + i);
    }
    for (int i = 0; i < n; i++) {
        printf("%d ", num[i]);
    }
    printf("\n");
    return 0;
}
