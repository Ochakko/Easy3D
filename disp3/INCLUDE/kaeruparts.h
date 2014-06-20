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
	HEAD1, // 
	HEAD2, // 
	HEAD00, //"00, 01, 02 は ０ の子"
	HEAD01, // 
	HEAD02, // 
	HEAD000, //"000, 001, 002 は 00 の子  "
	HEAD001, // 
	HEAD002, // 
	HEAD010, //"010, 011, 011 は 01 の子"
	HEAD011, // 
	HEAD012, // 
	ARML0, // 
	ARML1, // 
	ARML2, // 
	ARMR0, // 
	ARMR1, // 
	ARMR2, // 
	LEGL0, // 
	LEGL1, // 
	LEGL2, // 
	LEGR0, // 
	LEGR1, // 
	LEGR2, // 
	PIS, // 
	FIRE, // 
	CTLMAX
};

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

static char	lowctl_name[CTLMAX + 1][30] = {
	"cnone",
	"ctlall",
	"head0",
	"head1",
	"head2",
	"head00",
	"head01",
	"head02",
	"head000",
	"head001",
	"head002",
	"head010",
	"head011",
	"head012",
	"arml0",
	"arml1",
	"arml2",
	"armr0",
	"armr1",
	"armr2",
	"legl0",
	"legl1",
	"legl2",
	"legr0",
	"legr1",
	"legr2",
	"pis",
	"fire",
	"ctlmax"
};

static int	multctl_name[CTLMAX + 1] = {
	CTLNONE,
	CTLNONE, //必ず １個 必要  parts 全体の制御用
	CTLALL, //1 は O の子、 ２は １の子
	HEAD0, // 
	HEAD1, // 
	HEAD0, //"00, 01, 02 は ０ の子"
	HEAD0, // 
	HEAD0, // 
	HEAD00, //"000, 001, 002 は 00 の子  "
	HEAD00, // 
	HEAD00, // 
	HEAD01, //"010, 011, 011 は 01 の子"
	HEAD01, // 
	HEAD01, // 
	CTLALL, // 
	ARML0, // 
	ARML1, // 
	CTLALL, // 
	ARMR0, // 
	ARMR1, // 
	CTLALL, // 
	LEGL0, // 
	LEGL1, // 
	CTLALL, // 
	LEGR0, // 
	LEGR1, // 
	CTLALL, // 
	CTLALL, // 
	CTLNONE
};

static int ctl_groupno[CTLMAX + 1] = {
	// pause 作成時の Dialog の group 分けで 使用
	-1,
	0, //必ず １個 必要  parts 全体の制御用
	1, //1 は O の子、 ２は １の子
	1, // 
	1, // 
	2, //"00, 01, 02 は ０ の子"
	2, // 
	2, // 
	3, //"000, 001, 002 は 00 の子  "
	3, // 
	3, // 
	4, //"010, 011, 011 は 01 の子"
	4, // 
	4, // 
	5, // 
	5, // 
	5, // 
	6, // 
	6, // 
	6, // 
	7, // 
	7, // 
	7, // 
	8, // 
	8, // 
	8, // 
	9, // 
	9, // 
	-1
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
