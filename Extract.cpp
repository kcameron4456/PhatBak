#include "Extract.h"

Extract::Extract () {
    Repo = new RepoInfo (O.RepoDirName);
    Arch = new ArchiveRead (Repo, O.ArchDirName, O);
}

Extract::~Extract () {
    delete Arch;
    delete Repo;
}

void Extract::DoExtract () {
    Arch->DoExtract();
}
