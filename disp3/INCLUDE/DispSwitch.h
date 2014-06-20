#ifndef DISPSWITCHH
#define DISPSWITCHH


class CDispSwitch
{
public:
	CDispSwitch();
	~CDispSwitch();

public:
	int switchno;
	int state;
//	int boneno;//通常モーション時は使用しない。SetNewPoseByMOAで使用。
};

#endif