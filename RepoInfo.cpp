#include "RepoInfo.h"
#include "Logging.h"

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

RepoInfo::RepoInfo (const string &name) {
    Name = name;
    if (!fs::is_directory (Name))
        THROW_PBEXCEPTION_FMT ("Repo dir (%s) not found", Name.c_str());

    // check for repo consistancy
    string RepoId = Name + "/" + PHATBAK_REPO_ID;
    if (!fs::exists (RepoId))
        // TBD: optionally create repo if it doesn't exist
        THROW_PBEXCEPTION_FMT ("Repo Indentifier (%s) not found", RepoId.c_str());
}
