#include "Extract.h"
#include "Utils.h"
#include "Logging.h"

#include <filesystem>
namespace fs = std::filesystem;

Extract::Extract () {
    Repo = new RepoInfo (O.RepoDirName);

    // archive name defaults to latest found by RepoInfo
    string ArchName = O.ArchDirName;
    if (ArchName == "")
        ArchName = Repo->LatestArchName;
    assert (ArchName != "");

    cout << "Extracting from " << Repo->Name << "::" << ArchName << endl;

    Arch = new ArchiveRead (Repo, ArchName);
}

Extract::~Extract () {
    delete Arch;
    delete Repo;
}

void Extract::DoExtract () {
    // extract into new directory
    if (fs::exists (O.ExtractTarget))
        THROW_PBEXCEPTION ("Can't extract into existing directory: %s", O.ExtractTarget.c_str());
    Utils::CreateDir (O.ExtractTarget);

    Arch->DoExtract();
}
