#include "Create.h"
#include "Opts.h"
#include "Logging.h"

#include <string>
#include <vector>

Create::Create () {
    Repo.Init (O.RepoDirName);
    Arch.Init (&Repo, O.ArchDirName);
}

Create::~Create () {
}

void Create::DoCreate (const string &Name) {
printf ("Create::DoCreate %s\n", Name.c_str());
    // create local and archive file structures
    LiveFile LF (Name);
    ArchFile AF (&Arch);

    // remember info for each file
    FileList.push_back (Name);          // list of all files
    FileMap [Name] = LF;                // reference file info by name
    //TBD: test and set inode info properly for hardlinks
    Inodes [LF.Dev()][LF.INode()] = AF; // keep track of inodes we've seen for each device

    // create the archived file
    AF.Create(LF);

    // create sub dirs/files
    for (auto Sub : LF.GetSubs()) {
        DoCreate (Sub);
    }
}
