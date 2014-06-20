#define	PARAMTYPEH	1

enum {
	TYPE_float0, TYPE_float1, TYPE_floatDEG, TYPE_INT, TYPE_BOOL, TYPE_MAX
}; //paramtype

static char strparamtype[TYPE_MAX][15] = {
	"TYPE_float0", 
	"TYPE_float1", 
	"TYPE_floatDEG",
	"TYPE_INT", 
	"TYPE_BOOL"
};

static char strptype[TYPE_MAX][15] = {
	"float",	// TYPE_float0
	"float",	// TYPE_float1
	"float",	// TYPE_floatDEG
	"int",		// TYPE_INT
	"BOOL"		// TYPE_BOOL
};

static char strinitval[TYPE_MAX][15] = {
	"0.0f",		// TYPE_float0
	"1.0f",		// TYPE_float1
	"0.0f",
	"0",		// TYPE_INT
	"FALSE"		// TYPE_BOOL
};
