#include "Utils.h"
using namespace Utils;

#include <iostream>
using namespace std;

int main () {
    vector <string> StrList = {"aa", "bb", "cc"};
    cout << "Joined 1=" << JoinStrs (StrList) << endl;
    cout << "Joined 2=" << JoinStrs (StrList, "*") << endl;
    cout << "Joined 3=" << JoinStrs (StrList, "/") << endl;

    vector <string> Paths = {"/", "/ss/..", "/ss/../..", "ss///../tt/..//./" , "..", "../tartar"};
    for (auto Path : Paths) {
        string CPath = CanonizeFileName (Path);
        cout << "Path:" << Path << ": CPath:" << CPath << ":" << endl;
    }
}
