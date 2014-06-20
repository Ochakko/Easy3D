#define		KAERUPARTSH		1
	

//コントロール名( tlvName )
/*************************************************
* 変更する場合は mkcpp*\tlvhead.l も 変更 ！！！！！
*************************************************/ 

//EDITPOINT!!!ctl_name
enum ctl_name{
	CTLNONE,
	CTLALL, //必ず １個 必要  parts 全体の制御用
	HEAD0, //1 は O の子、 ２は １の子
	HEAD1, //*
	HEAD2, //*
	HEAD00, //00、 01、 02 は ０ の子 00のみscale付き
	HEAD01, //*
	HEAD02, //*
	HEAD000, //000、 001、 002 は 00 の子  
	HEAD001, //*
	HEAD002, //*
	HEAD010, //010、 011、 011 は 01 の子
	HEAD011, //*
	HEAD012, //*
	ARML0, //*
	ARML1, //*
	ARML2, //*
	ARMR0, //*
	ARMR1, //*
	ARMR2, //*
	LEGL0, //*
	LEGL1, //*
	LEGL2, //*
	LEGR0, //*
	LEGR1, //*
	LEGR2, //*
	PIS, //*
	FIRE, //*
	CTLMAX
};
//ENDEDIT

//EDITPOINT!!!strctl_name
static char	strctl_name[CTLMAX + 1][30] = {
	"CTLNONE",
	"CTLALL",
	"HEAD0",
	"HEAD1",
	"HEAD2",
	"HEAD00",
	"HEAD01",
	"HEAD02",
	"HEAD000",
	"HEAD001",
	"HEAD002",
	"HEAD010",
	"HEAD011",
	"HEAD012",
	"ARML0",
	"ARML1",
	"ARML2",
	"ARMR0",
	"ARMR1",
	"ARMR2",
	"LEGL0",
	"LEGL1",
	"LEGL2",
	"LEGR0",
	"LEGR1",
	"LEGR2",
	"PIS",
	"FIRE",
	"CTLMAX"
};
//ENDEDIT


enum{ KAMEHEAD0, KAMEHEAD1, 
	KAMEEYEL, KAMEEYER,
	KAMEMOUTH,
	KAMEARML0, KAMEARML1, KAMEHANDL,
	KAMEARMR0, KAMEARMR1, KAMEHANDR,
	KAMELEGL0, KAMELEGL1,
	KAMELEGR0, KAMELEGR1,
	KAMEBODY, 
	KAMETAIL,
	KAMEPARTSMAX };
