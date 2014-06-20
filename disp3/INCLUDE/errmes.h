#define	ERRMESH

#include <winbase.h>
#include <winerror.h>



class CErrMes
{
public:
	CErrMes();
	~CErrMes();

	void ErrStdOut( char* mes, HRESULT	hres );
	void ErrTrace( char* mes, HRESULT	hres );
	char*	GetErrorMes( HRESULT	hres );

};