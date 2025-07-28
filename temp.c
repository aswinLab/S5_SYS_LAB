#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILES 10
#define MAX_SUBDIRS 5

struct dir {
    char dir_name[20];
    char file[MAX_FILES][20];
    int findex;

    struct dir *subdirs[MAX_SUBDIRS];
    int subdir_count;

    struct dir *parent;
};

void createFile(struct dir *d) {
    if(d->findex < MAX_FILES){
        printf("Enter filename: ");
        scanf("%19s", d->file[d->findex]);
        d->findex++;
        printf("File created!\n");
    } else {
        printf("No more space available.\n");
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
            printf("File %s found at position %d.\n", filename, i + 1);
            return;
        }
    }
    printf("File not found.\n");
}

void listFiles(struct dir *d) {
    printf("Files in directory '%s':\n", d->dir_name);
    for(int i = 0; i < d->findex; i++){
        printf("%d. %s\n", i+1, d->file[i]);
    }
    if(d->findex == 0)
        printf("No files in directory.\n");
}

void renameFile(struct dir *d) {
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

void createSubdir(struct dir *parent) {
    if(parent->subdir_count >= MAX_SUBDIRS) {
        printf("Cannot create more subdirectories.\n");
        return;
    }

    struct dir *new_dir = (struct dir *)malloc(sizeof(struct dir));
    new_dir->findex = 0;
    new_dir->subdir_count = 0;
    new_dir->parent = parent;

    printf("Enter subdirectory name: ");
    scanf("%19s", new_dir->dir_name);

    parent->subdirs[parent->subdir_count++] = new_dir;
    printf("Subdirectory '%s' created.\n", new_dir->dir_name);
}

void listSubdirs(struct dir *d) {
    printf("Subdirectories of '%s':\n", d->dir_name);
    for(int i = 0; i < d->subdir_count; i++) {
        printf("- %s\n", d->subdirs[i]->dir_name);
    }
    if(d->subdir_count == 0)
        printf("No subdirectories.\n");
}

struct dir* enterSubdir(struct dir *current) {
    char name[20];
    printf("Enter subdirectory name: ");
    scanf("%19s", name);

    for(int i = 0; i < current->subdir_count; i++) {
        if(strcmp(current->subdirs[i]->dir_name, name) == 0) {
            printf("Entering subdirectory '%s'.\n", name);
            return current->subdirs[i];
        }
    }
    printf("Subdirectory not found.\n");
    return current;
}

void deleteDirectory(struct dir *current){
    char name[20];
    printf("Enter subdirectory name: ");
    scanf("%19s", name);
    
    if(strcmp(current->dir_name, name)==0){
    	printf("You cannot delete a dir when you are in it");
    	return;
    }

    for(int i = 0; i < current->subdir_count; i++) {
        if(strcmp(current->subdirs[i]->dir_name, name) == 0) {
            free(current->subdirs[i]);
            
            for(int j = i; j < current->subdir_count - 1; j++) {
                current->subdirs[j] = current->subdirs[j+1];
            }
            
            current->subdir_count--;
            printf("gone");
        }
    }
}

int main() {
    struct dir *root = (struct dir *)malloc(sizeof(struct dir));
    strcpy(root->dir_name, "root");
    root->findex = 0;
    root->subdir_count = 0;
    root->parent = NULL;

    struct dir *current = root;
    char option[3];

    while(1) {
        printf("\n[Current Directory: %s]\n", current->dir_name);
        printf("Options:\n");
        printf("n  - Create file\n");
        printf("d  - Delete file\n");
        printf("s  - Search file\n");
        printf("l  - List files\n");
        printf("rn - Rename file\n");
        printf("md - Make subdirectory\n");
        printf("ls - List subdirectories\n");
        printf("cd - Enter subdirectory\n");
        printf(".. - Go to parent directory\n");
        printf("rd  - Delete directory\n");
        printf("e  - Exit\n");
        printf("Enter option: ");
        scanf("%2s", option);

        if(strcmp(option, "n") == 0) {
            createFile(current);
        } else if(strcmp(option, "d") == 0) {
            deleteFile(current);
        } else if(strcmp(option, "s") == 0) {
            searchFile(current);
        } else if(strcmp(option, "l") == 0) {
            listFiles(current);
        } else if(strcmp(option, "rn") == 0) {
            renameFile(current);
        } else if(strcmp(option, "md") == 0) {
            createSubdir(current);
        } else if(strcmp(option, "ls") == 0) {
            listSubdirs(current);
        } else if(strcmp(option, "cd") == 0) {
            current = enterSubdir(current);
        } else if(strcmp(option, "rd") == 0) {
            deleteDirectory(current);
        } else if(strcmp(option, "..") == 0) {
            if(current->parent != NULL) {
                current = current->parent;
                printf("Returned to parent directory.\n");
            } else {
                printf("Already at root directory.\n");
            }
        } else if(strcmp(option, "e") == 0) {
            printf("Exiting...\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }

    // Optional: Free allocated memory (not handled here)

    return 0;
}

