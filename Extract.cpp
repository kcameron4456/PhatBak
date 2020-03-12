#include "Extract.h"

Extract::Extract () {
    Repo = new RepoInfo (O.RepoDirName);
    Arch = new ArchiveRead (Repo, O);
}

Extract::~Extract () {
    free (Arch);
    free (Repo);
}

void Extract::DoExtract () {
    Arch->DoExtract();
}
