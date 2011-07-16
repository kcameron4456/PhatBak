#ifndef EXTRACT_H
#define EXTRACT_H

#include "Opts.h"
#include "RepoInfo.h"
#include "Archive.h"

class Extract {
    RepoInfo     *Repo;
    ArchiveRead  *Arch;

    public:
     Extract ();
    ~Extract ();
    void DoExtract ();
};

#endif // EXTRACT_H
