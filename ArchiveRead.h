#ifndef ARCHIVEREAD_H
#define ARCHIVEREAD_H

#include "LiveFile.h"
#include "RepoInfo.h"
#include "BlockList.h"
#include "Opts.h"

#include <string>
#include <vector>
#include <stdio.h>
#include <mutex>
using namespace std;

class Archive {
    public:
    RepoInfo     *Repo;
    string        Name;
    string        ArchDirPath;
    string        ListPath;
    string        OptionsPath;
    string        FinfoDirPath;
    string        ChunkDirPath;
    BlockList    *FInfoBlocks;
    BlockList    *ChunkBlocks;

     Archive (RepoInfo *repo);
    ~Archive ();
};

class ArchiveRead : public Archive {
    Opts O;
    void ParseOptions();

    public:
     ArchiveRead (RepoInfo *repo, Opts &o);
    ~ArchiveRead ();
    void DoExtract();
};

class ChunkInfo {
    public:
    char         Comp;
    BlockIdxType Idx;
    string       Hash;
    ChunkInfo (char comp, BlockIdxType idx, const string& hash);
    ChunkInfo (const string &finfoline);
};

class ArchFile {
    public:
    Archive           *Arch;
    string             Name;
    BlockIdxType       InfoBlkNum;
    char               InfoBlkComp;
    string             InfoBlkHash;
    mutex              Mtx;
    string             Stats;
    vector <ChunkInfo> DataChnks;

     ArchFile (Archive *arch);
    ~ArchFile ();
};

class ArchFileRead : public ArchFile {
    public:

     ArchFileRead (ArchiveRead *arch, const string &ListEntry, uint64_t LineNo);
    ~ArchFileRead ();

    void DoExtract ();
};

#endif // ARCHIVEREAD_H
