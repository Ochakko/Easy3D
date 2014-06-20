#define	CHARPARAMH	1

#ifndef	PARAMTYPEH
	#include "paramtype.h"
#endif

#include "kaeruparts.h"

//EDITPOINT!!!CHARPARAM
typedef	struct	charparam{

// CTRL CTLALL
	float	shiftXctlall;
	float	shiftYctlall;
	float	shiftZctlall;
	float	rotXctlall;
	float	rotYctlall;
	float	rotZctlall;

// CTRL HEAD0
	float	shiftXhead0;
	float	shiftYhead0;
	float	shiftZhead0;
	float	rotXhead0;
	float	rotYhead0;
	float	rotZhead0;

// CTRL HEAD1
	float	shiftXhead1;
	float	shiftYhead1;
	float	shiftZhead1;
	float	rotXhead1;
	float	rotYhead1;
	float	rotZhead1;

// CTRL HEAD2
	float	shiftXhead2;
	float	shiftYhead2;
	float	shiftZhead2;
	float	rotXhead2;
	float	rotYhead2;
	float	rotZhead2;

// CTRL HEAD00
	float	shiftXhead00;
	float	shiftYhead00;
	float	shiftZhead00;
	float	rotXhead00;
	float	rotYhead00;
	float	rotZhead00;
	float	scaleXhead00;
	float	scaleYhead00;
	float	scaleZhead00;

// CTRL HEAD01
	float	shiftXhead01;
	float	shiftYhead01;
	float	shiftZhead01;
	float	rotXhead01;
	float	rotYhead01;
	float	rotZhead01;

// CTRL HEAD02
	float	shiftXhead02;
	float	shiftYhead02;
	float	shiftZhead02;
	float	rotXhead02;
	float	rotYhead02;
	float	rotZhead02;

// CTRL HEAD000
	float	shiftXhead000;
	float	shiftYhead000;
	float	shiftZhead000;
	float	rotXhead000;
	float	rotYhead000;
	float	rotZhead000;

// CTRL HEAD001
	float	shiftXhead001;
	float	shiftYhead001;
	float	shiftZhead001;
	float	rotXhead001;
	float	rotYhead001;
	float	rotZhead001;

// CTRL HEAD002
	float	shiftXhead002;
	float	shiftYhead002;
	float	shiftZhead002;
	float	rotXhead002;
	float	rotYhead002;
	float	rotZhead002;

// CTRL HEAD010
	float	shiftXhead010;
	float	shiftYhead010;
	float	shiftZhead010;
	float	rotXhead010;
	float	rotYhead010;
	float	rotZhead010;

// CTRL HEAD011
	float	shiftXhead011;
	float	shiftYhead011;
	float	shiftZhead011;
	float	rotXhead011;
	float	rotYhead011;
	float	rotZhead011;

// CTRL HEAD012
	float	shiftXhead012;
	float	shiftYhead012;
	float	shiftZhead012;
	float	rotXhead012;
	float	rotYhead012;
	float	rotZhead012;

// CTRL ARML0
	float	shiftXarml0;
	float	shiftYarml0;
	float	shiftZarml0;
	float	rotXarml0;
	float	rotYarml0;
	float	rotZarml0;

// CTRL ARML1
	float	shiftXarml1;
	float	shiftYarml1;
	float	shiftZarml1;
	float	rotXarml1;
	float	rotYarml1;
	float	rotZarml1;

// CTRL ARML2
	float	shiftXarml2;
	float	shiftYarml2;
	float	shiftZarml2;
	float	rotXarml2;
	float	rotYarml2;
	float	rotZarml2;

// CTRL ARMR0
	float	shiftXarmr0;
	float	shiftYarmr0;
	float	shiftZarmr0;
	float	rotXarmr0;
	float	rotYarmr0;
	float	rotZarmr0;

// CTRL ARMR1
	float	shiftXarmr1;
	float	shiftYarmr1;
	float	shiftZarmr1;
	float	rotXarmr1;
	float	rotYarmr1;
	float	rotZarmr1;

// CTRL ARMR2
	float	shiftXarmr2;
	float	shiftYarmr2;
	float	shiftZarmr2;
	float	rotXarmr2;
	float	rotYarmr2;
	float	rotZarmr2;

// CTRL LEGL0
	float	shiftXlegl0;
	float	shiftYlegl0;
	float	shiftZlegl0;
	float	rotXlegl0;
	float	rotYlegl0;
	float	rotZlegl0;

// CTRL LEGL1
	float	shiftXlegl1;
	float	shiftYlegl1;
	float	shiftZlegl1;
	float	rotXlegl1;
	float	rotYlegl1;
	float	rotZlegl1;

// CTRL LEGL2
	float	shiftXlegl2;
	float	shiftYlegl2;
	float	shiftZlegl2;
	float	rotXlegl2;
	float	rotYlegl2;
	float	rotZlegl2;

// CTRL LEGR0
	float	shiftXlegr0;
	float	shiftYlegr0;
	float	shiftZlegr0;
	float	rotXlegr0;
	float	rotYlegr0;
	float	rotZlegr0;

// CTRL LEGR1
	float	shiftXlegr1;
	float	shiftYlegr1;
	float	shiftZlegr1;
	float	rotXlegr1;
	float	rotYlegr1;
	float	rotZlegr1;

// CTRL LEGR2
	float	shiftXlegr2;
	float	shiftYlegr2;
	float	shiftZlegr2;
	float	rotXlegr2;
	float	rotYlegr2;
	float	rotZlegr2;

// CTRL PIS
	float	scaleXpis;
	float	scaleYpis;
	float	scaleZpis;
	BOOL	actionpis;

// CTRL FIRE
	float	scaleXfire;
	float	scaleYfire;
	float	scaleZfire;
	float	rotXfire;
	float	rotYfire;
	float	rotZfire;
	BOOL	actionfire;
} CHARPARAM;

#define	CPARAMMAX	164
static char	strcharparam[CPARAMMAX][50] = {

// CTRL CTLALL
	"shiftXctlall",
	"shiftYctlall",
	"shiftZctlall",
	"rotXctlall",
	"rotYctlall",
	"rotZctlall",

// CTRL HEAD0
	"shiftXhead0",
	"shiftYhead0",
	"shiftZhead0",
	"rotXhead0",
	"rotYhead0",
	"rotZhead0",

// CTRL HEAD1
	"shiftXhead1",
	"shiftYhead1",
	"shiftZhead1",
	"rotXhead1",
	"rotYhead1",
	"rotZhead1",

// CTRL HEAD2
	"shiftXhead2",
	"shiftYhead2",
	"shiftZhead2",
	"rotXhead2",
	"rotYhead2",
	"rotZhead2",

// CTRL HEAD00
	"shiftXhead00",
	"shiftYhead00",
	"shiftZhead00",
	"rotXhead00",
	"rotYhead00",
	"rotZhead00",
	"scaleXhead00",
	"scaleYhead00",
	"scaleZhead00",

// CTRL HEAD01
	"shiftXhead01",
	"shiftYhead01",
	"shiftZhead01",
	"rotXhead01",
	"rotYhead01",
	"rotZhead01",

// CTRL HEAD02
	"shiftXhead02",
	"shiftYhead02",
	"shiftZhead02",
	"rotXhead02",
	"rotYhead02",
	"rotZhead02",

// CTRL HEAD000
	"shiftXhead000",
	"shiftYhead000",
	"shiftZhead000",
	"rotXhead000",
	"rotYhead000",
	"rotZhead000",

// CTRL HEAD001
	"shiftXhead001",
	"shiftYhead001",
	"shiftZhead001",
	"rotXhead001",
	"rotYhead001",
	"rotZhead001",

// CTRL HEAD002
	"shiftXhead002",
	"shiftYhead002",
	"shiftZhead002",
	"rotXhead002",
	"rotYhead002",
	"rotZhead002",

// CTRL HEAD010
	"shiftXhead010",
	"shiftYhead010",
	"shiftZhead010",
	"rotXhead010",
	"rotYhead010",
	"rotZhead010",

// CTRL HEAD011
	"shiftXhead011",
	"shiftYhead011",
	"shiftZhead011",
	"rotXhead011",
	"rotYhead011",
	"rotZhead011",

// CTRL HEAD012
	"shiftXhead012",
	"shiftYhead012",
	"shiftZhead012",
	"rotXhead012",
	"rotYhead012",
	"rotZhead012",

// CTRL ARML0
	"shiftXarml0",
	"shiftYarml0",
	"shiftZarml0",
	"rotXarml0",
	"rotYarml0",
	"rotZarml0",

// CTRL ARML1
	"shiftXarml1",
	"shiftYarml1",
	"shiftZarml1",
	"rotXarml1",
	"rotYarml1",
	"rotZarml1",

// CTRL ARML2
	"shiftXarml2",
	"shiftYarml2",
	"shiftZarml2",
	"rotXarml2",
	"rotYarml2",
	"rotZarml2",

// CTRL ARMR0
	"shiftXarmr0",
	"shiftYarmr0",
	"shiftZarmr0",
	"rotXarmr0",
	"rotYarmr0",
	"rotZarmr0",

// CTRL ARMR1
	"shiftXarmr1",
	"shiftYarmr1",
	"shiftZarmr1",
	"rotXarmr1",
	"rotYarmr1",
	"rotZarmr1",

// CTRL ARMR2
	"shiftXarmr2",
	"shiftYarmr2",
	"shiftZarmr2",
	"rotXarmr2",
	"rotYarmr2",
	"rotZarmr2",

// CTRL LEGL0
	"shiftXlegl0",
	"shiftYlegl0",
	"shiftZlegl0",
	"rotXlegl0",
	"rotYlegl0",
	"rotZlegl0",

// CTRL LEGL1
	"shiftXlegl1",
	"shiftYlegl1",
	"shiftZlegl1",
	"rotXlegl1",
	"rotYlegl1",
	"rotZlegl1",

// CTRL LEGL2
	"shiftXlegl2",
	"shiftYlegl2",
	"shiftZlegl2",
	"rotXlegl2",
	"rotYlegl2",
	"rotZlegl2",

// CTRL LEGR0
	"shiftXlegr0",
	"shiftYlegr0",
	"shiftZlegr0",
	"rotXlegr0",
	"rotYlegr0",
	"rotZlegr0",

// CTRL LEGR1
	"shiftXlegr1",
	"shiftYlegr1",
	"shiftZlegr1",
	"rotXlegr1",
	"rotYlegr1",
	"rotZlegr1",

// CTRL LEGR2
	"shiftXlegr2",
	"shiftYlegr2",
	"shiftZlegr2",
	"rotXlegr2",
	"rotYlegr2",
	"rotZlegr2",

// CTRL PIS
	"scaleXpis",
	"scaleYpis",
	"scaleZpis",
	"actionpis",

// CTRL FIRE
	"scaleXfire",
	"scaleYfire",
	"scaleZfire",
	"rotXfire",
	"rotYfire",
	"rotZfire",
	"actionfire"
};

static int charparamtype[CPARAMMAX] = {

// CTRL CTLALL
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD0
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD1
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD2
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD00
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_float1,
	TYPE_float1,
	TYPE_float1,

// CTRL HEAD01
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD02
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD000
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD001
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD002
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD010
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD011
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL HEAD012
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL ARML0
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL ARML1
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL ARML2
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL ARMR0
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL ARMR1
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL ARMR2
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL LEGL0
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL LEGL1
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL LEGL2
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL LEGR0
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL LEGR1
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL LEGR2
	TYPE_float0,
	TYPE_float0,
	TYPE_float0,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,

// CTRL PIS
	TYPE_float1,
	TYPE_float1,
	TYPE_float1,
	TYPE_BOOL,

// CTRL FIRE
	TYPE_float1,
	TYPE_float1,
	TYPE_float1,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_floatDEG,
	TYPE_BOOL
};

static int charparamnum[CTLMAX + 1] = {
	0, // CTLNONE
	6, // CTRL CTLALL
	6, // CTRL HEAD0
	6, // CTRL HEAD1
	6, // CTRL HEAD2
	9, // CTRL HEAD00
	6, // CTRL HEAD01
	6, // CTRL HEAD02
	6, // CTRL HEAD000
	6, // CTRL HEAD001
	6, // CTRL HEAD002
	6, // CTRL HEAD010
	6, // CTRL HEAD011
	6, // CTRL HEAD012
	6, // CTRL ARML0
	6, // CTRL ARML1
	6, // CTRL ARML2
	6, // CTRL ARMR0
	6, // CTRL ARMR1
	6, // CTRL ARMR2
	6, // CTRL LEGL0
	6, // CTRL LEGL1
	6, // CTRL LEGL2
	6, // CTRL LEGR0
	6, // CTRL LEGR1
	6, // CTRL LEGR2
	4, // CTRL PIS
	7, // CTRL FIRE
	0 // CTLMAX
};

static int charphead[CTLMAX + 1] = {
	0, // CTLNONE
	0, // CTRL CTLALL
	6, // CTRL HEAD0
	12, // CTRL HEAD1
	18, // CTRL HEAD2
	24, // CTRL HEAD00
	33, // CTRL HEAD01
	39, // CTRL HEAD02
	45, // CTRL HEAD000
	51, // CTRL HEAD001
	57, // CTRL HEAD002
	63, // CTRL HEAD010
	69, // CTRL HEAD011
	75, // CTRL HEAD012
	81, // CTRL ARML0
	87, // CTRL ARML1
	93, // CTRL ARML2
	99, // CTRL ARMR0
	105, // CTRL ARMR1
	111, // CTRL ARMR2
	117, // CTRL LEGL0
	123, // CTRL LEGL1
	129, // CTRL LEGL2
	135, // CTRL LEGR0
	141, // CTRL LEGR1
	147, // CTRL LEGR2
	153, // CTRL PIS
	157, // CTRL FIRE
	0 // CTLMAX
};
//ENDEDIT


typedef struct enemy_param{
	float	headsize;
	float	neckwidth;
	float	neckheight;
	float	bodywidth0;	//â°ïù
	float	bodywidth1;	//ëOïù
	float	bodyheight;
	float	armwidth0;
	float	armheight0;
	float	armwidth1;
	float	armheight1;
	float	handsize;
	float	legwidth0;
	float	legheight0;
	float	legwidth1;
	float	legheight1;
	float	legwidth2;	//Çöï˚å¸í∑Ç≥
	float	legheight2;
	float	bodyscale[6];
} ENEMYPARAM;
