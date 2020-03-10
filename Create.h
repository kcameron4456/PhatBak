#ifndef CREATE_H
#define CREATE_H

#include "LiveFile.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

class Create {
    public:
    vector <string>           FileList;  // ordered list of files to be backed up
    map    <string, LiveFile> FileMap;   // lookup file structs by name
    map <uint32_t, map <uint64_t, uint32_t>> Inodes; // link counts for each inode of each block device

     Create ();
    ~Create ();
    void DoCreate (const string &Dir);
    void DoCreateRaw (const string &RawName);
};

#endif // CREATE_H
