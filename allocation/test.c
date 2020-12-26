#include <stdio.h>
#include <stdlib.h>

#include "dyh.h"

typedef struct {
    int x;
    double y;
} node;

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

    node *nd = (node *)dyh_malloc(sizeof(node) * n);
    for (int i = 0; i < n; i++) {
        scanf("%d%lf", &nd[i].x, &nd[i].y);
    }
    for (int i = 0; i < n; i++) {
        printf("%d %.2lf\n", nd[i].x, nd[i].y);
    }

    return 0;
}
