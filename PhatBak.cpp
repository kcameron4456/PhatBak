#include "Types.h"
#include "Create.h"
#include "Extract.h"
#include "LiveFile.h"
#include "Logging.h"
#include "Opts.h"
#include "Utils.h"
#include "ThreadPool.h"

#include <stdio.h>

int main (int argc, const char **argv) {
    try {
        O.ParseCmdLine (argc, argv);

        ThreadPool.AddThreads (O.NumThreads);

        if (O.Operation == Opts::DoCreate) {
            Create *C = new Create;
            C->DoCreate ();
            delete C;
        } else if (O.Operation == Opts::DoExtract) {
            Extract *E = new Extract;
            E->DoExtract ();
            delete E;
        } else if (O.Operation == Opts::DoList) {
            auto Repo = new RepoInfo (O.RepoDirName);
            if (O.ArchDirName.size()) {
                auto Arch = new ArchiveRead (Repo, O.ArchDirName);
                Arch->DoList();
                delete Arch;
            } else {
                Repo->DoList();
            }
            delete Repo;
        } else if (O.Operation == Opts::DoTest) {
            auto Repo = new RepoInfo (O.RepoDirName);

            // archive name defaults to latest found by RepoInfo
            string ArchName = O.ArchDirName;
            if (ArchName == "")
                ArchName = Repo->LatestArchName;
            assert (ArchName != "");
            cout << "Testing integrity of " << Repo->Name << "::" << ArchName << endl;

            auto Arch = new ArchiveRead (Repo, ArchName);
            Arch->DoTest();

            delete Arch;
            delete Repo;
        } else if (O.Operation == Opts::DoCompare) {
            auto Repo = new RepoInfo (O.RepoDirName);

            // archive name defaults to latest found by RepoInfo
            string ArchName = O.ArchDirName;
            if (ArchName == "")
                ArchName = Repo->LatestArchName;
            assert (ArchName != "");
            cout << "Comparing " << Repo->Name << "::" << ArchName << " against existing files" << endl;

            auto Arch = new ArchiveRead (Repo, ArchName);
            Arch->DoCompare();

            delete Arch;
            delete Repo;
        } else if (O.Operation == Opts::DoShowLatest) {
            auto Repo = new RepoInfo (O.RepoDirName);
            cout << Repo->LatestArchName << endl;
            delete Repo;
        } else if (O.Operation == Opts::DoInit) {
            auto Repo = new RepoInfo (O.RepoDirName);
            delete Repo;
        } else {
            THROW_PBEXCEPTION ("Operation %d not supported", O.Operation);
        }
    }

    // handle exceptions
    catch (const char *msg) {
        fprintf (stderr, "%s\n", msg);
        return 1;
    }
    catch (PB_Exception &PBE) {
        PBE.Handle();
    }

    return 0;
}
