#ifndef SNAPSHOT_TREE_H
#define SNAPSHOT_TREE_H

/**
 * This Object is the raw data of a files that had been tracked
*/
struct raw_object {
    char* file;
    unsigned char* chunks;
    unsigned int chunks_len;
};

/**
 * This object is a n-tree which holds the current raw datas (the child-files) and others n-trees which representing directories
 * /!\ : this is used for simplifying the tracking and untracking of files
*/
struct snapshot_tree {
    char* dir;  
    struct snapshot_tree** children;
    struct raw_object** raw_objects;
};


/**
 * Make a raw data object of a given file
 * the filename should hold the relative path of the current repository
*/
struct raw_object* make_raw_object(char* filename);
/**
 * Make a file tracked for the current commit
*/
void track_raw_object(struct snapshot_tree* snapshot_tree, char* filename);

#endif 
