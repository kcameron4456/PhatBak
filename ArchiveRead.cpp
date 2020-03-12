#include "ArchiveRead.h"
#include "Logging.h"
#include "Utils.h"

#include<string>
#include<fstream>

void ArchiveRead::Init (RepoInfo *repo, Opts &o) {
    Repo        = repo;
    O           = o;
    Name        = O.ArchDirName;
    ArchDirPath = Repo->Name + "/" + Name;
    ParseOptions ();
}

void ArchiveRead::DoExtract () {
    // open the file list
    string ListFileN = ArchDirPath + "/List";
    ifstream ListFile (ListFileN);
    if (ListFile.fail())
        THROW_PBEXCEPTION_IO ("ArchiveRead::DoExtract - Can't open %s for read", ListFileN.c_str());

    // parse and extract all the entries in the list
    string FileLine;
    uint64_t LineCnt = 0;
    while (getline (ListFile, FileLine)) {
        LineCnt ++;

        // seperate filename from attributes
        vector <string> FirstCut = Utils::SplitStr (FileLine, " /../ ");
        if (FirstCut.size() != 2)
            THROW_PBEXCEPTION_FMT ("%s:%llu has bad format", ListFileN.c_str(), LineCnt);
        string FileName = FirstCut[0];

        // separate fields of rhs
        vector <string> RHSToks = Utils::SplitStr (FirstCut[1], " ");
        if (RHSToks.size() != 3)
            THROW_PBEXCEPTION_FMT ("%s:%llu has bad format", ListFileN.c_str(), LineCnt);
        BlockIdxType BlockIdx = stoull (RHSToks[0]);
        char         Comp     = RHSToks[1][0];
        string       Hash     = RHSToks[2];
printf ("ArchiveRead::DoExtract (%llu) %s %llu %c %s\n", LineCnt, FileName.c_str(), (uint64_t) BlockIdx, Comp, Hash.c_str());
    }

    ListFile.close();
}

void ArchiveRead::ParseOptions () {
    // extract options from the archive file
    string OptsFileN = ArchDirPath + "/Options";
    ifstream OptsFile (OptsFileN);
    if (OptsFile.fail())
        THROW_PBEXCEPTION_IO ("Can't open %s for read", OptsFileN.c_str());
    string OptLine;
    while (getline (OptsFile, OptLine)) {
        // skip non-option lines
        if (OptLine.find ("=") == string::npos)
            continue;

        // get rid of leading whitespace
        auto NameBegin = OptLine.find_first_not_of (" \t");

        // option name is everything before next whitespace
        auto NameEnd   = OptLine.find_first_of (" \t", NameBegin);
        string OptName = OptLine.substr (NameBegin, NameEnd - NameBegin);

        // get option value
        auto ValBegin = OptLine.find_first_not_of (" \t=", NameEnd);
        string Val    = OptLine.substr (ValBegin);

        if (OptName == "BlockNumDigits" ) O.BlockNumDigits  = stoull(Val);
        if (OptName == "BlockNumModulus") O.BlockNumModulus = stoull(Val);
        if (OptName == "ChunkSize"      ) O.ChunkSize       = stoull(Val);
        if (OptName == "HashType"       ) O.HashType        = HashNameToEnum (Val) ;
    }

    O.Print();
    OptsFile.close();
}
