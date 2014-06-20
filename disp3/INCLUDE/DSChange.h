#ifndef DSCHANGEH
#define DSCHANGEH


class CDispSwitch;

class CDSChange
{
public:
	CDSChange();
	~CDSChange();

	int SetDSChange( int srcchange );

private:
	int InitParams();
	int DestroyObjs();

public:
	CDispSwitch* m_dsptr;
	int m_change;

};

#endif