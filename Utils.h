#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <fstream>
using namespace std;

namespace Utils {
    // split string into in parts delimited by pattern
    vector <string> SplitStr (string Src, const string &Pat);

    // remove all white space from beginning and end of string
    void   TrimStr (string *Str);
    string TrimStr (string  Str);

    // create full path name to file without extraneous relatime paths 
    string CanonizeFileName (const string &RawName);

    // join multiple strings into one with optional separater
    string JoinStrs (const vector <string> &Parts, const string &Sep = "");

    // open file stream for input
    fstream OpenReadStream (const string &Name);

    // open file stream for output
    fstream OpenWriteStream (const string &Name);
}

#endif // UTILS_H
