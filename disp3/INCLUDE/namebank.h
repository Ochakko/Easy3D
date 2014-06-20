#include <crtdbg.h>
#include <coef.h> // namebankmode

class CNameBank
{
public:
	CNameBank();
	~CNameBank();

	// �I�I�I�@srcno�́A�V���A���ł͂Ȃ��A�z��̃C���f�b�N�X�B�@�I�I�I
	char* operator() ( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(hname + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}
	};
	int GetIntData( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(pidata + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};
	int GetIntData2( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(pidata2 + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};
	int GetIntData3( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(pidata3 + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};
	int GetIntData4( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(pidata4 + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};
	int GetType( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(Type + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};
	int GetFMT( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(pfmt + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	}

	int GetSerial( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(pserial + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};
	int GetDirtyFlag( int srcno ){
		if( (srcno >= 0) && (srcno < namenum) ){
			return *(dirtyflag + srcno);
		}else{
			_ASSERT( 0 );
			return 0;
		}		
	};



	int AddName( char* srcname, int srcidata = 0, int srcidata2 = 0, int lookintdata = 0 );
	int FindName( char* srcname );//�I�I�I�Ԓl�̓V���A���ł͂Ȃ��A�z��̃C���f�b�N�X�I�I�I
	int FindName( char* srcname, int srcint );//�I�I�I�Ԓl�̓V���A���ł͂Ȃ��A�z��̃C���f�b�N�X�I�I�I

	int GetNameBySerial( int srcseri, char** ppname, int* ptrans );
	int GetIndexBySerial( int srcseri );

	int ResetDirtyFlag();
	int SetDirtyFlag( char* srcname, int srcint );

	int Rename( int srcindex, char* srcname );

	int SetFMT( int srcindex, int srcfmt );

private:
	void InitParams();
	void DestroyObjs();

public:
	char** hname;
	int* pidata;//transparent
	int* pidata2;//pool
	int* pserial;
	int* dirtyflag;

	int* pidata3;
	int* pidata4;
	SIZE* porgsize;
	int* Type;
	int* pfmt;//D3DFMT_*

	int namenum;
};