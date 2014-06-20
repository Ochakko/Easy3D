#ifndef FILTERTYPEH
#define FILTERTYPEH

enum {
	AVGF_NONE			= 0,
	AVGF_MOVING,				//�ړ�����
	AVGF_WEIGHTED_MOVING,		//���d�ړ�����
	AVGF_GAUSSIAN,				//�K�E�V�A�� 
	AVGF_MAX
};

static char stravgf[AVGF_MAX][20] =
{
	"�Ȃ�",
	"�ړ�����",
	"���d�ړ�����",
	"�K�E�V�A��"
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