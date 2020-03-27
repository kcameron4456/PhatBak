#ifndef CREATE_H
#define CREATE_H

#include "LiveFile.h"
#include "RepoInfo.h"
#include "Archive.h"

#include <string>
#include <vector>
#include <map>
using namespace std;

class Create {
    public:
    RepoInfo       *Repo;      // information about current repository
    ArchiveCreate  *Arch;      // information about current archive
    ArchiveBase    *ArchBase;  // information about base archive
    map <u32, map <u64, InodeInfo*>> Inodes; // archive info for each inode of each block device
    mutex                            InodesMtx; // avoid races accessing Inodes

     Create ();
    ~Create ();
    void DoCreate ();
    void DoCreate (const string &Dir, bool Recurse = 1);
};

#endif // CREATE_H
