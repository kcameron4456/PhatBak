#include "LiveFile.h"
#include "Logging.h"
#include "Opts.h"

#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

LiveFile::LiveFile (const string &name) {
    Name = name;
    Init ();
}

void LiveFile::Init () {
    FD = -1;

    // get file info
    if (lstat (Name.c_str(), &Stats) < 0)
        THROW_PBEXCEPTION_IO ("Can't stat file: %s", Name.c_str());

    // type-specific actions
           if (IsFile  ()) {
    } else if (IsDir   ()) {
    } else if (IsFifo  ()) {
    } else if (IsSocket()) {
    } else if (IsSLink ()) {
        char Targ [1000];
        int TargSize = readlink (Name.c_str(), Targ, sizeof(Targ));
        if (TargSize < 0)
            THROW_PBEXCEPTION_IO ("Can't read symbolic link '%s'", Name.c_str());
        if (TargSize >= 1000)
            THROW_PBEXCEPTION ("Symbolic link '%s' too long", Name.c_str());
        Target = string (Targ, TargSize);
    } else {
        THROW_PBEXCEPTION ("File %s is an unsupported type", Name.c_str());
    }
}

vector <string> LiveFile::GetSubs () {
    vector <string> Subs;
    if (!IsDir())
        return Subs;
    for (const auto& entry : filesystem::directory_iterator(Name)) {
        Subs.push_back (Name + "/" + entry.path().filename().string());
    }
    return Subs;
}

string LiveFile::MakeInfoHeader () const {
    stringstream res;
    res << "dev:"   << Stats.st_dev  << " ";
    res << "inode:" << INode()       << " ";
    res << "mode:"  << Stats.st_mode << " ";
    res << "uid:"   << Stats.st_uid  << " ";
    res << "gid:"   << Stats.st_gid  << " ";
    res << "size:"  << Stats.st_size << " ";
    res << "mtime:" << mTime()       << " ";
    res << "ctime:" << cTime();

    return res.str();
}

void SplitFileName (const string &RawName, string &Path, string &Name) {
    // split into path and leaf names
    // leaf is just the part after the last "/"
    int LastSlash = RawName.rfind ('/');
    Path = RawName.substr (0,LastSlash);
    Name = RawName.substr (LastSlash+1);
}

string CanonizeFileName (const string &RawName) {
    // this is a painful way of doing it but I couldn't
    // find a standard library version that wouldn't follow
    // symbolic links
//printf ("CanonizeFileName: RawName=%s\n", RawName.c_str());
    string CanName = RawName;

    // prepend current directory to name, if needed
    if ((CanName.substr (0,1)) != "/") {
        char cwd [10000];
        if (getcwd (cwd, sizeof(cwd)) == NULL) {
            // FLAG ERROR
        }
//printf ("CanonizeFileName: cwd=%s\n", cwd);
        CanName = string(cwd) + "/" + CanName;
    }

    // tokenize by spliting on "/"
    // do you believe this ancient c function is better than
    // anything in the c++ string library?
    char *WorkStr  = strdup (CanName.c_str());
    char *WorkStr2 = WorkStr;
    char *WorkSave = WorkStr;
//printf ("CanonizeFileName: WorkStr=%s\n", WorkStr);
    vector <string> Toks;
    char *tok = strtok_r (WorkStr, "/", &WorkStr2);
    while (tok) {
        Toks.push_back(tok);
//printf ("CanonizeFileName: tok=%s\n", tok);
        tok = strtok_r (NULL, "/", &WorkStr2);
    }
    free (WorkSave);

    // get rid of all instances of "."
    bool changed;
    do {
        changed = 0;
        for (auto itr = Toks.begin(); itr != Toks.end(); itr++) {
            if (*itr == ".") {
                Toks.erase (itr);
                changed = 1;
                break;
            }
        }
    } while (changed);

    // make multiple passes through the tokens if needed to clean up 'dir/..'
    do {
        changed = 0;
        for (auto itr = Toks.begin(); itr != Toks.end(); itr++) {
            if (*itr == "..") {
                Toks.erase (itr);
                if (itr != Toks.begin())
                    Toks.erase (itr - 1);
                changed = 1;
                break;
            }
        }
    } while (changed);

    // assemble the full path from the components
    string FullName;
    for (auto itr = Toks.begin(); itr != Toks.end(); itr++)
        FullName += "/" + *itr;
    if (FullName == "")
        FullName = "/";
//printf ("CanonizeFileName: FullName=%s\n", FullName.c_str());

    return FullName;
}

void LiveFile::Close () {
    if (close (FD) < 0)
         THROW_PBEXCEPTION_IO ("Can't close file: %s ", Name.c_str());
    FD = -1;
}

void LiveFile::OpenRead () {
    if ((FD = open (Name.c_str(), O_RDONLY)) < 0)
         THROW_PBEXCEPTION_IO ("Can't open file for read: %s ", Name.c_str());
}

void LiveFile::OpenWrite () {
    if ((FD = open (Name.c_str(), O_WRONLY)) < 0)
         THROW_PBEXCEPTION_IO ("Can't open file for write: %s ", Name.c_str());
}

int LiveFile::Read (char *Buf, int ReqSize) {
    int RdSize;
    if ((RdSize = read (FD, Buf, ReqSize)) < 0)
         THROW_PBEXCEPTION_IO ("Error reading from: " + Name);
    return RdSize;
}

int LiveFile::ReadChunk (char *Buf) {
    return Read (Buf, O.ChunkSize);
}
