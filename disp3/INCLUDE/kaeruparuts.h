#define		KAERUPARTSH		1
	

//�R���g���[����( tlvName )
/*************************************************
* �ύX����ꍇ�� mkcpp*\tlvhead.l �� �ύX �I�I�I�I�I
*************************************************/ 

//EDITPOINT!!!ctl_name
enum ctl_name{
	CTLNONE,
	CTLALL, //�K�� �P�� �K�v  parts �S�̂̐���p
	HEAD0, //1 �� O �̎q�A �Q�� �P�̎q
	HEAD1, //*
	HEAD2, //*
	HEAD00, //00�A 01�A 02 �� �O �̎q 00�̂�scale�t��
	HEAD01, //*
	HEAD02, //*
	HEAD000, //000�A 001�A 002 �� 00 �̎q  
	HEAD001, //*
	HEAD002, //*
	HEAD010, //010�A 011�A 011 �� 01 �̎q
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
