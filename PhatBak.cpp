#include "Create.h"
#include "LiveFile.h"
#include "Logging.h"

#include <stdio.h>

int main () {
    Create C;

    try {
        C.DoCreate (CanonizeFileName ("../tartar"));
        //C.DoCreate (CanonizeFileName ("../.."));
    }

    // handle exceptions
    catch (const char *msg) {
        fprintf (stderr, "%s\n", msg);
        return 1;
    }
    catch (PB_Exception &PBE) {
        PBE.Handle();
    }

    return 0;
}
