#include <stdafx.h> //ÇæÇ›Å[
#include <boneinfo.h>

#include <math.h>

CBoneInfo::CBoneInfo()
{
	ZeroMemory( &bonestart, sizeof( CVec3f ) );
	ZeroMemory( &boneend, sizeof( CVec3f ) );
	boneleng = 0.0f;
	ZeroMemory( &bonevec, sizeof( CVec3f ) );
	jointno = -1;
	parentjoint = -1;

	ZeroMemory( &bonecenter, sizeof( D3DXVECTOR3 ) );

	childno = -1;
}

CBoneInfo::CBoneInfo( CBoneInfo* srcbi )
{
	bonestart.CopyData( &(srcbi->bonestart) );
	boneend.CopyData( &(srcbi->boneend) );
	boneleng = srcbi->boneleng;
	bonevec.CopyData( &(srcbi->bonevec) );
	jointno = srcbi->jointno;
	parentjoint = srcbi->parentjoint;

	childno = srcbi->childno;
}

CBoneInfo::~CBoneInfo()
{

}