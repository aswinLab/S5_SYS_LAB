#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dir {
    char dir_name[20];
    char file[10][20];
    int findex;  // number of files currently
};

void create(struct dir *d) {
    if(d->findex < 10){
        printf("Enter filename: ");
        scanf("%19s", d->file[d->findex]);
        d->findex++;
        printf("File created!\n");
    } else {
        printf("No more space available :(\n");
    }
}

void deleteFile(struct dir *d) {
    if(d->findex == 0){
        printf("No files to delete.\n");
        return;
    }
    char filename[20];
    printf("Enter filename to delete: ");
    scanf("%19s", filename);

    int i, found = 0;
    for(i = 0; i < d->findex; i++){
        if(strcmp(d->file[i], filename) == 0){
            found = 1;
            break;
        }
    }
    if(found){
        for(int j = i; j < d->findex - 1; j++){
            strcpy(d->file[j], d->file[j+1]);
        }
        d->findex--;
        printf("File deleted successfully.\n");
    } else {
        printf("File not found.\n");
    }
}

void searchFile(struct dir *d) {
    if(d->findex == 0){
        printf("No files available.\n");
        return;
    }
    char filename[20];
    printf("Enter filename to search: ");
    scanf("%19s", filename);

    for(int i = 0; i < d->findex; i++){
        if(strcmp(d->file[i], filename) == 0){
            printf("File '%s' found.\n", filename);
            return;
        }
    }
    printf("File not found.\n");
}

void listFiles(struct dir *d) {
    if(d->findex == 0){
        printf("No files in directory.\n");
        return;
    }
    printf("Files in directory '%s':\n", d->dir_name);
    for(int i = 0; i < d->findex; i++){
        printf("%d. %s\n", i+1, d->file[i]);
    }
}

void renameFile(struct dir *d) {
    if(d->findex == 0){
        printf("No files available to rename.\n");
        return;
    }
    char oldname[20], newname[20];
    printf("Enter filename to rename: ");
    scanf("%19s", oldname);

    int i, found = 0;
    for(i = 0; i < d->findex; i++){
        if(strcmp(d->file[i], oldname) == 0){
            found = 1;
            break;
        }
    }

    if(found){
        printf("Enter new filename: ");
        scanf("%19s", newname);
        strcpy(d->file[i], newname);
        printf("File renamed successfully.\n");
    } else {
        printf("File not found.\n");
    }
}

int main() {
    struct dir d;
    d.findex = 0;

    printf("Enter directory name: ");
    scanf("%19s", d.dir_name);

    char option[3];  // use string because "rn" option is two chars
    while(1) {
        printf("\nOptions:\n");
        printf("Create a new file - n\nDelete a file - d\nSearch for a file - s\nList all available files - l\nRename a file - rn\nExit - e\n");
        printf("Enter option: ");
        scanf("%2s", option);

        if(strcmp(option, "e") == 0) {
            printf("Exiting...\n");
            break;
        } else if(strcmp(option, "n") == 0) {
            create(&d);
        } else if(strcmp(option, "d") == 0) {
            deleteFile(&d);
        } else if(strcmp(option, "s") == 0) {
            searchFile(&d);
        } else if(strcmp(option, "l") == 0) {
            listFiles(&d);
        } else if(strcmp(option, "rn") == 0) {
            renameFile(&d);
        } else {
            printf("Invalid option. Try again.\n");
        }
    }
    return 0;
}


/*  Enter directory name: aswin

Options:
Create a new file - n
Delete a file - d
Search for a file - s
List all available files - l
Rename a file - rn
Exit - e
Enter option: n
Enter filename: aswin
File created!

Enter option: n
Enter filename: a
File created!

Enter option: n
Enter filename: b
File created!

Enter option: l
Files in directory 'aswin':
1. aswin
2. a
3. b

Enter option: s
Enter filename to search: aswin
File 'aswin' found.

Enter option: d
Enter filename to delete: aswin
File deleted successfully.

Files in directory 'aswin':
1. a
2. b

Enter option: rn
Enter filename to rename: a
Enter new filename: c
File renamed successfully.

*/



