#include <windows.h>
#include <d3dx9.h>

class CIncludeManager : public ID3DXInclude
{
public:
    CIncludeManager();
	~CIncludeManager();
    STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    STDMETHOD(Close)(LPCVOID pData);
};


