#include "Create.h"

#include <string>
#include <vector>

Create::Create () {
}

Create::~Create () {
}

void Create::DoCreate (const string &Name) {
printf ("Create::DoCreate %s\n", Name.c_str());
    LiveFile LF (Name);

    FileList.push_back (Name);         // list of all files
    FileMap [Name] = LF;               // reference file info by name
    Inodes [LF.Dev()][LF.INode()] ++;  // keep track of inodes we've seen for each device

    if (LF.IsDir ()) {
        vector <string> Subs = LF.GetSubs();
        for (auto Sub : Subs) {
            DoCreate (Sub);
        }
    }
}
