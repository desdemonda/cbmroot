/**
 * \file loadlib.C
 * \brief Macro for loading basic and cbmroot libraries.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
void loadlibs()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");
}
