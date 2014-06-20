#ifndef MOTIONPOINTH
#define MOTIONPOINTH

#include <Quaternion.h>
#include <D3DX9.h>

#include <crtdbg.h>

class CMotionPoint
{
public:
	CMotionPoint();
	~CMotionPoint();
	
	int InitParams();

	//int SetParams( int srcframe,
	//	float srcrotx, float srcroty, float srcrotz,
	//	float srcmvx, float srcmvy, float srcmvz );//old

	//int SetParams( int srcframe, D3DXVECTOR3 axis, float axisrot,
	//	float srcmvx, float srcmvy, float srcmvz );
	int SetParams( int srcframe, CQuaternion srcq, 
		float srcmvx, float srcmvy, float srcmvz );


	int SetFrameNo( int srcframe );
	int SetRotParams( float srcrotx, float srcroty, float srcrotz );
	int SetMoveParams( float srcmvx, float srcmvy, float srcmvz );

	//int SetQuaternion();
	//int SetQuaternionIK();

	int SetQuaternion( CQuaternion srcq );
	int MultQuaternion( CQuaternion srcq );


	int InitConvFlag();

	//int FramenoCmp( CMotionPoint* cmpmp );
	//int FramenoCmp( int cmpno );

	int AddToPrev( CMotionPoint* addmp );
	int AddToNext( CMotionPoint* addmp );
	int LeaveFromChain();

	int FillUpMotionPoint( CMotionPoint* srcstartmp, CMotionPoint* srcendmp, int framenum, int framecnt, int interp );

	int CopyMotion( CMotionPoint* srcmp );


	__inline int CMotionPoint::FramenoCmp( CMotionPoint* cmpmp )
	{
		int cmpno;
		_ASSERT( cmpmp );
		cmpno = cmpmp->m_frameno;

		if( m_frameno > cmpno ){
			return 1;
		}else if( m_frameno < cmpno ){
			return -1;
		}else{
			return 0;
		}
	};

	__inline int CMotionPoint::FramenoCmp( int cmpno )
	{
		if( m_frameno > cmpno ){
			return 1;
		}else if( m_frameno < cmpno ){
			return -1;
		}else{
			return 0;
		}
	};


public:
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// クラスのインスタンス丸ごと、quaに書き出すので、メンバーの追加は、MotionPoint2 の　reserved 変数にすること！！！！
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	int ishead;
	int m_frameno;
	float m_rotx, m_roty, m_rotz;//！！！　IKバージョン以降は、使用しないが、ファイルの互換性のために残しておく。
	float m_mvx, m_mvy, m_mvz;

	//D3DXVECTOR3 m_axis;
	//float m_axisrot;

	CQuaternion m_q;

	//int convflagx, convflagy, convflagz;

	CMotionPoint* prev;
	CMotionPoint* next;
};

#endif