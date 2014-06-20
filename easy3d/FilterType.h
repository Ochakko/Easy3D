#ifndef FILTERTYPEH
#define FILTERTYPEH

enum {
	AVGF_NONE			= 0,
	AVGF_MOVING,				//移動平均
	AVGF_WEIGHTED_MOVING,		//加重移動平均
	AVGF_GAUSSIAN,				//ガウシアン 
	AVGF_MAX
};

static char stravgf[AVGF_MAX][20] =
{
	"なし",
	"移動平均",
	"加重移動平均",
	"ガウシアン"
};

static int avgfsize[10] =
{
	3, 5, 7, 9, 11,
	13, 15, 17, 19, 21
};

static  char stravgfsize[10][5] =
{
	"3", "5", "7", "9", "11",
	"13", "15", "17", "19", "21"
};

#endif