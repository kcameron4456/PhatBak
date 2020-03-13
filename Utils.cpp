#include "Utils.h"
#include "Logging.h"

#include <unistd.h>
#include <stdio.h>

vector <string> Utils::SplitStr (string Src, const string &Pat) {
    vector <string> Toks;
    while (Src.size()) {
        auto TokEnd = Src.find (Pat);
        string Tok = Src.substr (0, TokEnd);
        Toks.push_back(Tok);
        Src.erase (0, Tok.size() + Pat.size());
    }
    return Toks;
}

void Utils::TrimStr (string  *Str) {
    static const char *WhiteSpace = " \t\n";

    // strip leading whitespace
    size_t Begin = Str->find_first_not_of (WhiteSpace);
    Str->erase (0, Begin);

    // strip trailing whitespace
    size_t End = Str->find_last_not_of (WhiteSpace);
    Str->erase (End + 1);
}

string Utils::TrimStr (string Str) {
    Utils::TrimStr (&Str);
    return Str;
}

string Utils::CanonizeFileName (const string &RawName) {
    string FullName;

    // prepend current directory to name, if needed
    if (RawName[0] == '/') {
        FullName = RawName;
    } else {
        char cwd [4000];
        if (getcwd (cwd, sizeof(cwd)) == NULL)
            THROW_PBEXCEPTION ("getcwd failed: ");
        FullName = string(cwd) + "/" + RawName;
    }

    // tokenize by spliting on "/"
    vector <string> Toks = Utils::SplitStr (FullName, "/");

    // discard all instances of "", ".", and "dir/.."
    for (auto itr = Toks.begin(); itr < Toks.end(); itr++) {
        if (*itr == "" || *itr == ".") {
            Toks.erase (itr);
            itr --;
        } else if (*itr == "..") {
            if (itr == Toks.begin()) {
                // handle .. at beginning
                Toks.erase (itr);
                itr --;
            } else {
                Toks.erase (itr-1, itr+1);
                itr -= 2;
            }
        }
    }

    // assemble the full path from the components
    string CanName = "/" + Utils::JoinStrs (Toks, "/");
    return CanName;
}

string Utils::JoinStrs (const vector <string> &Parts, const string &Sep) {
    string Joined;
    bool first = 1;
    for (auto &Part : Parts) {
        if (!first)
            Joined += Sep;
        Joined += Part;
        first = false;
    }
    return Joined;
}

fstream Utils::OpenReadStream (const string &Name) {
    fstream Strm (Name.c_str(), fstream::in);
    if (Strm.fail())
        THROW_PBEXCEPTION_IO ("Can't open %s for read", Name.c_str());
    return Strm;
}

fstream Utils::OpenWriteStream (const string &Name) {
    fstream Strm (Name.c_str(), fstream::out);
    if (Strm.fail())
        THROW_PBEXCEPTION_IO ("Can't open %s for write", Name.c_str());
    return Strm;
}
