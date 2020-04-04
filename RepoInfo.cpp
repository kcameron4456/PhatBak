#include "RepoInfo.h"
#include "Logging.h"
#include "Utils.h"

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

RepoInfo::RepoInfo (const string &name) {
    Name = Utils::CanonizeFileNameNoCWD (name);
    if (!fs::is_directory (Name)) {
        if (O.Operation == O.DoInit) {
            Utils::CreateDir (Name);
        } else {
            ERROR ("Repo dir (%s) not found\n", Name.c_str());
        }
    }

    // check for repo consistancy
    string RepoId = Name + "/" + PHATBAK_REPO_ID;
    if (!fs::exists (RepoId)) {
        if (O.Operation == O.DoInit) {
            Utils::Touch (RepoId);
        } else {
            ERROR ("Repo Indentifier (%s) not found\n", RepoId.c_str());
        }
    }

    // check for previous base archive 
    LatestArchName = "";
    if (!O.Rebase) {
        vecstr SubDirs, SubFiles;
        Utils::SlurpDir (Name, SubDirs, SubFiles);

        // find most recent standard archive (i.e. name is time in standaridized format)
        for (auto SubDir : SubDirs) {
            if (  !fs::exists (Name + "/" + SubDir + "/" + PHATBAK_ARCH_ID)
               || !fs::exists (Name + "/" + SubDir + "/" + PHATBAK_ARCH_FINISHED)
               )
                continue;
            static const string Pattern = "XXXX_XX_XX_XXXX_XX";
            if (SubDir.size() != Pattern.size())
                continue;
            string TempSubDir = SubDir;
            for (char &c : TempSubDir)
                if (c >= '0' && c <= '9')
                    c = 'X';
            if (TempSubDir != Pattern)
                continue;
            if (SubDir > LatestArchName)
                LatestArchName = SubDir;
        }
    }
}

void RepoInfo::Finish (const string &ArchName) {
}

void RepoInfo::DoList () {
    vecstr SubDirs, SubFiles;
    Utils::SlurpDir (Name, SubDirs, SubFiles);
    vecstr Archs;
    for (auto SubDir : SubDirs)
        if (fs::exists (Name + "/" + SubDir + "/" + PHATBAK_ARCH_ID))
            Archs.push_back (Name + "::" + SubDir);
    sort (Archs.begin(), Archs.end());
    for (auto &Arch : Archs)
        cout << Arch << endl;
}
