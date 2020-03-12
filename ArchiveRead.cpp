#include "ArchiveRead.h"
#include "Logging.h"
#include "Utils.h"
using namespace Utils;

#include<string>
#include<fstream>

Archive::Archive(RepoInfo *repo) {
    Repo         = repo;
    Name         = O.ArchDirName;
    ArchDirPath  = Repo->Name + "/" + Name;
    ListPath     = ArchDirPath + "/List";
    OptionsPath  = ArchDirPath + "/Options";
    FinfoDirPath = ArchDirPath + "/FInfo";
    ChunkDirPath = ArchDirPath + "/Chunks";
}

Archive::~Archive() {
}

ArchiveRead::ArchiveRead (RepoInfo *repo, Opts &o) : Archive (repo) {
    O = o;
    ParseOptions ();

    FInfoBlocks = new BlockList (FinfoDirPath, O);
    ChunkBlocks = new BlockList (ChunkDirPath, O);
}

ArchiveRead::~ArchiveRead() {
}

void ArchiveRead::ParseOptions () {
    // extract options from the archive file
    string OptsFileN = ArchDirPath + "/Options";
    ifstream OptsFile (OptsFileN);
    if (OptsFile.fail())
        THROW_PBEXCEPTION_IO ("Can't open %s for read", OptsFileN.c_str());
    string OptLine;
    while (getline (OptsFile, OptLine)) {
        // split into name/value pairs
        vector <string> Toks = SplitStr (OptLine, "=");

        // skip non-option lines
        if (Toks.size() != 2)
            continue;

        // clean up tokens
        for (auto &Tok : Toks)
            TrimStr (&Tok);

        // rename tokens
        string &OptName = Toks[0];
        string &OptVal  = Toks[1];

             if (OptName == "BlockNumDigits" ) O.BlockNumDigits  = stoull         (OptVal);
        else if (OptName == "BlockNumModulus") O.BlockNumModulus = stoull         (OptVal);
        else if (OptName == "ChunkSize"      ) O.ChunkSize       = stoull         (OptVal);
        else if (OptName == "HashType"       ) O.HashType        = HashNameToEnum (OptVal);
    }

    O.Print();
    OptsFile.close();
}

void ArchiveRead::DoExtract () {
    // open the file list
    string ListFileN = ArchDirPath + "/List";
    ifstream ListFile (ListFileN);
    if (ListFile.fail())
        THROW_PBEXCEPTION_IO ("ArchiveRead::DoExtract - Can't open %s for read", ListFileN.c_str());

    // parse and extract all the entries in the list
    string FileLine;
    uint64_t LineNo = 0;
    while (getline (ListFile, FileLine)) {
        LineNo ++;
        //ArchFileRead *AF = new ArchFileRead (this, FileLine, LineNo);
    }

    ListFile.close();
}

ArchFile::ArchFile (Archive *arch) {
    Arch = arch;
}

ArchFile::~ArchFile () {
}

ArchFileRead::ArchFileRead (ArchiveRead *arch, const string &ListEntry, uint64_t LineNo) : ArchFile (arch) {
    // parse file list entry
    // separate filename from attributes
    vector <string> FirstCut = SplitStr (ListEntry, " /../ ");
    if (FirstCut.size() != 2)
        THROW_PBEXCEPTION_FMT ("%s:%llu has bad format", Arch->ListPath.c_str(), LineNo);
    Name = FirstCut[0];

    // separate fields of rhs
    vector <string> RHSToks = SplitStr (FirstCut[1], " ");
    if (RHSToks.size() != 3)
        THROW_PBEXCEPTION_FMT ("%s:%llu has bad format", Arch->ListPath.c_str(), LineNo);
    InfoBlkNum  = stoull (RHSToks[0]);
    InfoBlkComp = RHSToks[1][0];
    InfoBlkHash = RHSToks[2];

    // extract information from 
    auto FInfoStrm = Arch->FInfoBlocks->OpenReadStream (InfoBlkNum);

    FInfoStrm.close();
}

ArchFileRead::~ArchFileRead () {
}
