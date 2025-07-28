#include <stdio.h>
#include <stdlib.h>

int main() {
    int i, j, temp, n, h, req[30], seek = 0, pos, disk = 200, dir;

    printf("Enter the head position: ");
    scanf("%d", &h);

    printf("Enter the number of requests: ");
    scanf("%d", &n);

    printf("Enter the requests: ");
    for (i = 0; i < n; i++) {
        scanf("%d", &req[i]);
    }

    // Add head, start, and end tracks
    req[n] = h;
    req[n + 1] = 0;
    req[n + 2] = disk - 1;
    n += 3;

    // Sort the requests
    for (i = 0; i < n -1; i++) {
        for (j = i +1; j < n; j++) {
            if (req[i] > req[j]) {
                temp = req[i];
                req[i] = req[j];
                req[j] = temp;
            }
        }
    }

    // Find position of head in sorted array
    for (pos = 0; pos < n; pos++) {
        if (req[pos] == h) {
            break;
        }
    }

    printf("Enter direction for C-SCAN (0 for right, 1 for left): ");
    scanf("%d", &dir);

    printf("\nSeek Sequence:\n");

    if (dir == 0) {
        // Move right from head to end
        for (i = pos; i < n -1; i++) {
            printf("%d -> ", req[i]);
            seek += abs(req[i + 1] - req[i]);
        }
        printf("%d\n", req[n - 1]);

        // Jump from end to start
        seek += abs((disk - 1) - 0);
        printf("Jump from %d to %d\n", disk -1, 0);

        // Move right from start to head-1
        for (i = 0; i < pos -1; i++) {
            printf("%d -> ", req[i]);
            seek += abs(req[i + 1] - req[i]);
        }
        if (pos -1 >= 0) printf("%d\n", req[pos -1]);
    }
    else if (dir == 1) {
        // Move left from head to start
        for (i = pos; i > 0; i--) {
            printf("%d -> ", req[i]);
            seek += abs(req[i] - req[i - 1]);
        }
        printf("%d\n", req[0]);

        // Jump from start to end
        seek += abs((disk - 1) - 0);
        printf("Jump from %d to %d\n", 0, disk - 1);

        // Move left from end to head+1
        for (i = n -1; i > pos +1; i--) {
            printf("%d -> ", req[i]);
            seek += abs(req[i] - req[i -1]);
        }
        if (pos +1 < n) printf("%d\n", req[pos +1]);
    }
    else {
        printf("Invalid direction input.\n");
        return 1;
    }

    printf("\nTotal Seek Time: %d\n", seek);
    return 0;
}
