#include <stdio.h>
#include <stdlib.h>

int *sort(int n, int *arr) {
    int temp = 0;
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-i-1; j++) {
            if(arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
    return arr;
}

int scan_q(int n, int *arr, int head, char choice) {
    int *s_arr = sort(n, arr);
    int mid = 0;
    int seek = 0;
    int distance = 0;

    for(int i = 0; i < n; i++) {
        if(head < s_arr[i]) {
            mid = i;
            break;
        }
    }
	
    if(choice == 'l') {
        int i = mid - 1;
        while(i >= 0) {
            distance = abs(head - s_arr[i]);
            seek += distance;
            printf("%d\n", seek);
            head = s_arr[i];
            i--;
        }
        seek = seek + 11;
        head = 0;
        for(int i = 0; i<n; i++){
       	    distance = abs(head - s_arr[i]);
            seek += distance;
            printf("%d\n", seek);
            head = s_arr[i];
        }
        return seek;
    }

    if(choice == 'r') {
        int i = mid;
        while(i < n) {
            distance = abs(s_arr[i] - head);
            seek += distance;
            printf("%d\n", seek);
            head = s_arr[i];
            i++;
        }
        seek = seek + 23;
        head = 199;
        for(int i = n-1; i>=0; i--){
       	    distance = abs(head - s_arr[i]);
            seek += distance;
            printf("%d", seek);
            head = s_arr[i];
        }
        return seek;
    }
}

int main() {
    int que[100];
    int head;
    int n = 0;
    char choice;

    printf("Enter scan direction (l/r): ");
    scanf(" %c", &choice);

    printf("Enter initial head position: ");
    scanf("%d", &head);

    printf("Enter number of requests: ");
    scanf("%d", &n);

    printf("Enter the requests: ");
    for(int i = 0; i < n; i++) {
        scanf("%d", &que[i]);
    }

    int seek = scan_q(n, que, head, choice);
    printf("Total seek time: %d\n", seek);

    return 0;
}

