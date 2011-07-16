#include "Opts.h"
#include "Utils.h"
#include "Logging.h"

#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <filesystem>
using namespace std;
namespace fs = filesystem;

// global options declaration
Opts O;

// default version - overriden in Makefile from git release tag
#ifndef VERSION_MAJOR
    #define VERSION_MAJOR 0
#endif
#ifndef VERSION_MINOR
    #define VERSION_MINOR 5
#endif

#define MAXARGVALLEN 1000   // maximum length of individual command line arguments

static void PrintHelp(int rval=0) {
    fprintf (stderr, "%s\n", R"(
Usage: PhatBak <Operation> [Options] Repo[::Archive] [file/dir list]
use "man PhatBak" for details
)");

    exit (rval);
}

static void ArgError (const char *arg, const char *nxtarg = NULL) {
    const char *a = nxtarg ? nxtarg : "";
    fprintf (stderr, "ERROR: Illegal Argument: %s %s\n", arg, a);
    PrintHelp(1);
}

void PrintOpHelp () {
    fprintf (stderr, "ERROR: Exactly one flag of type {c -c x -x t -t} must be given\n");
    PrintHelp(1);
}

static void PrintVersion() {
    printf ("Version:  %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
    exit (0);
}

// maybe use this for sizes someday
#if 0
static int ParseSize (const char *SizeStr) {
    const char *Save = SizeStr;
    int Num = 0;

    // handle number
    while (*SizeStr) {
        if (*SizeStr >= '0' && *SizeStr <= '9') {
            Num *= 10;
            Num += *SizeStr - '0';
        } else {
            break;
        }
        SizeStr++;
    }

    // handle units
    if (!strcmp (SizeStr, "KiB"))
        Num *= 1024;
    else if (!strcmp (SizeStr, "MiB"))
        Num *= 1024 * 1024;
    else if (!strcmp (SizeStr, "GiB"))
        Num *= 1024 * 1024 * 1024;
    else if (!strcmp (SizeStr, "KB"))
        Num *= 1000 ;
    else if (!strcmp (SizeStr, "MB"))
        Num *= 1000 * 1000;
    else if (!strcmp (SizeStr, "GB"))
        Num *= 1000 * 1000 * 1000;
    else if (*SizeStr)
        ArgError (Save);

    if (!Num)
        ArgError (Save);

    return Num;
}
#endif

// macro to skip to the next argument
#define NXTARG { \
    argidx++; \
    if (argidx >= argc) \
        ArgError (arg); \
    arg = argv[argidx]; \
    if (strlen (arg) >= (MAXARGVALLEN-2)) \
        ArgError (arg); \
}

// parse command line options
#define TESTOP {if (Operation != DoUndef) PrintOpHelp();}
void Opts::ParseCmdLine (const int argc, const char *argv[]) {
    // apply defaults
    VersionMajor    = VERSION_MAJOR;
    VersionMinor    = VERSION_MINOR;
    Operation       = DoUndef;
    ShowFiles       = 0;
    NumThreads      = 100;
    CompType        = CompType_ZTSD;
    CompLevel       = 2;
    ChunkSize       = 1 << 18;
    HashType        = HashType_MD5;
    ExtractTarget   = "PhatBakExtract";
    DebugPrint      = 0;
    BlockNumModulus = 100;
    Rebase          = false;
    CWD             = fs::canonical(fs::current_path()); // resolves symlinks

    // save command line
    int argidx;
    for (argidx=0; argidx < argc; argidx++) {
        if (argidx)
            CmdLine += " ";
        CmdLine += argv[argidx];
    }

    // first arg is operation
    // allow abbreviations
    if (argc < 2 || argv[1][0] == '\0')
        ERROR ("No operation given\n");
    const char *arg = argv[1];
    vecstr MatchNames;
    for (OpEnum i = (OpEnum)((int)DoUndef+1); i < DoVoid; i = (OpEnum)((int)i + 1))
        if (OpText(i).find (arg) == 0)
            MatchNames.push_back(OpText(i));
    if (MatchNames.size() == 0)
        ERROR ("Operation (%s) not recognized\n", arg);
    if (MatchNames.size() > 1)
        ERROR ("Operation (%s) is ambiguous, use one of %s\n", arg, Utils::JoinStrs(MatchNames, ",").c_str());
         if (OpText (DoInit      ) == MatchNames[0]) Operation = DoInit      ;
    else if (OpText (DoCreate    ) == MatchNames[0]) Operation = DoCreate    ;
    else if (OpText (DoExtract   ) == MatchNames[0]) Operation = DoExtract   ;
    else if (OpText (DoTest      ) == MatchNames[0]) Operation = DoTest      ;
    else if (OpText (DoCompare   ) == MatchNames[0]) Operation = DoCompare   ;
    else if (OpText (DoList      ) == MatchNames[0]) Operation = DoList      ;
    else if (OpText (DoShowLatest) == MatchNames[0]) Operation = DoShowLatest;
    else if (OpText (DoVersion   ) == MatchNames[0]) Operation = DoVersion   ;

    // basic operation must be set
    if (Operation == DoUndef)
        THROW_PBEXCEPTION ("Unrecognized OpText: %s", MatchNames[0]);

    // process version here
    if (Operation == DoVersion)
        PrintVersion();

    // step through all "-" and "--" args
    for (argidx = 2; argidx < argc; argidx++) {
        const char *arg = argv[argidx];

        // test non-minus arg
        if (arg[0] != '-') {
            // it's the end of minus args
            break;
        }

        // begin generic argument parsing
        #define PARSE_MinusVal(Name, ValFmt, ValPtr, Action) { \
            if (!strcmp (arg, Name)) { \
                NXTARG \
                if (sscanf (arg, ValFmt, ValPtr) != 1) \
                    ArgError (arg, arg); \
                Action \
                continue; \
            } \
        }
        #define PARSE_MinusStr(Name, ValNam, Action) { \
            if (!strcmp (arg, Name)) { \
                NXTARG \
                ValNam = arg; \
                Action \
                continue; \
            } \
        }
        #define PARSE_MinusFlg(Name, Test, ValNam, Val, Action) { \
            if (!strcmp (arg, Name)) { \
                Test \
                ValNam = Val; \
                Action \
                continue; \
            } \
        }

        //int TmpInt = 0;
        PARSE_MinusFlg ("-v"                ,, ShowFiles  , 1,)
        PARSE_MinusFlg ("-D"                ,, ShowFiles=ArchDiag, 1, )
        PARSE_MinusVal ("-T"                ,"%d", &NumThreads,)
        PARSE_MinusStr ("--CompType"        , arg, CompType = Comp::CompNameToEnum(arg);)
        PARSE_MinusVal ("--CompLevel"       ,"%d", &CompLevel,)
        PARSE_MinusStr ("--HashType"        , arg, HashType = HashNameToEnum(arg);)
        PARSE_MinusVal ("--ChunkSize"       ,"%d", &ChunkSize,)
        PARSE_MinusVal ("--BlockNumModulus" ,"%d", &BlockNumModulus,)
        PARSE_MinusFlg ("--rebase"          ,, Rebase    , 1,)
        PARSE_MinusStr ("--BaseArchive"     ,  BaseArchive,  )
        PARSE_MinusFlg ("-h"                ,, arg       , arg, PrintHelp();)
        PARSE_MinusFlg ("-help"             ,, arg       , arg, PrintHelp();)
        PARSE_MinusFlg ("--help"            ,, arg       , arg, PrintHelp();)
        PARSE_MinusFlg ("--debug"           ,, DebugPrint,   1, ;)

        ArgError(arg);
    }

    // first remaining arg is the repo/archive name
    if (argidx >= argc) {
        printf ("No Repo::Archive argument given\n");
        PrintHelp(1);
    }
    string RepoArchName = argv[argidx++];
    vector <string> Parts = Utils::SplitStr (RepoArchName, "::");
    if (Parts.size() > 2)
        PrintHelp(1);
    RepoDirName = Parts[0];
    if (Parts.size() >= 2)
        ArchDirName = Parts[1];

    // remaining args are file/dir names for create or extract
    for (; argidx < argc; argidx++) {
        string FileName = argv[argidx];

        // strip trailing /
        if (!FileName.size())
            continue;
        if (FileName[FileName.size()-1] == '/')
            FileName.resize(FileName.size()-1);
        if (!FileName.size())
            FileName = "/";

        FileArgs.push_back (FileName);
    }

    // default to cwd for create
    if (!FileArgs.size() && Operation == DoCreate)
        FileArgs.push_back (".");
}

Opts::Opts () {
    StartTime    = Utils::TimeNowNs ();
    StartTimeTxt = Utils::NsToText (StartTime);
}

void Opts::Print (fstream &F) {
    F << OptsString ();
}

void Opts::Print (ostream &F) {
    F << OptsString ();
}

string Opts::OptsString () {
    stringstream F;
    F << "Options:\n";
    F << "   CmdLine         = " << CmdLine                         << endl;
    F << "   Operation       = " << OpText()                        << endl;
    F << "   FileArgs        = " << Utils::JoinStrs (FileArgs, " ") << endl;
    F << "   CWD             = " << CWD                             << endl;
    F << "   RepoDirName     = " << RepoDirName                     << endl;
    F << "   ArchDirName     = " << ArchDirName                     << endl;
    F << "   BaseArchive     = " << BaseArchive                     << endl;
    F << "   BlockNumModulus = " << BlockNumModulus                 << endl;
    F << "   ChunkSize       = " << ChunkSize                       << endl;
    F << "   HashType        = " << HashNames[HashType]             << endl;
    F << "   CompType        = " << CompNames[CompType]             << endl;
    F << "   CompLevel       = " << CompLevel                       << endl;
    F << "   ShowFiles       = " << ShowFiles                       << endl;
    F << "   DebugPrint      = " << DebugPrint                      << endl;
    return F.str();
}
