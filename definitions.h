#ifndef __DEFINES__
#define __DEFINES__


// The purpose of this file is to defines the macros
// Used in all systems parts


// ----------------- General macros -----------------
#define __ERROR__   -1
#define __OK__      0
#define DEFAULT_TTL 60
// -------------- End general macros -----------------

// ---------- Inter program communication ------------

// OPCODES
#define SHOW_TABLE  0
#define RES_IP      1
#define ADD_LINE    2
#define DEL_LINE    3
#define SET_TTL     4
#define LIST_IFCES  5

// -------- End inter program communication ----------

#endif
