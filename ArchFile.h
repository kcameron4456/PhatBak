#ifndef ARCHFILE_H
#define ARCHFILE_H

#include "LiveFile.h"

#include <string>
#include <vector>
#include <stdio.h>
using namespace std;

class ArchFile {
    public:
    string   Name;
    uint64_t InfoBlkN;
    string   Stats;
    FILE    *LogFile;
    vector <uint64_t> DataBlkNs;

    ArchFile () {} // empty constructor
    ArchFile (const LiveFile &lf);
};

#endif // ARCHFILE_H
