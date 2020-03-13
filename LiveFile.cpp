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
    for (const auto& entry : filesystem::directory_iterator(Name))
        Subs.push_back (Name + "/" + entry.path().filename().string());
    return Subs;
}

string LiveFile::MakeInfoHeader () const {
    stringstream res;
    //res << "dev:"   << hex << Stats.st_dev  << " ";  Not Needed?
    //res << "inode:" << hex << INode()       << " ";
    res << "mode:"  << hex << Stats.st_mode << " ";
    res << "uid:"   << hex << Stats.st_uid  << " ";
    res << "gid:"   << hex << Stats.st_gid  << " ";
    res << "size:"  <<        Stats.st_size << " "; // keep size in decimal to make it easier to read and debug
    res << "mtime:" << hex << mTime()       << " ";
    res << "ctime:" << hex << cTime();

    return res.str();
}

void SplitFileName (const string &RawName, string &Path, string &Name) {
    // split into path and leaf names
    // leaf is just the part after the last "/"
    int LastSlash = RawName.rfind ('/');
    Path = RawName.substr (0,LastSlash);
    Name = RawName.substr (LastSlash+1);
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
