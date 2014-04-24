// Everything.c has to include every c file which is used in the bricklet
// plugin. This file is the only file that is being compiled later on with
// the compiler.

// This is unfortunately needed, since the arm-gcc does only inline the GOT
// and PLT (global offset table and procedure linkage table) in functions that
// are local to the entry function (i.e., they are in the same file as the
// entry function) when the -PIE (position independent code) flag is used.

// We obviously need PIE's, since the addresses are unknown at compile time.
// Therefore we need to make each used function local to the entry function.
// One possibility to do this is to use the -fwhole-program compiler flag,
// which essentially does the same as this file. However, this flag is
// deprecated in the gcc version currently used and will be removed in
// future versions and probably replaced by the LTO (linker time optimization)
// flag.

// Thus, at the moment this silly everything.c approach seems to be the
// most sane thing to do. In the future there might be a -no-gotplt option
// (as it is already available in mips-gcc) or the LTO flag might do the trick.
// In any of these cases, the everything.c will be removed. So the bricklet
// plugins should also always be compilable the "normal way".


#include "brickletlib/bricklet_entry.c"
#include "brickletlib/bricklet_simple.c"
#include "nfc-rfid.c"
