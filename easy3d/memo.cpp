	char fname1[MAX_PATH];
	if( tempname1 ){
		strcpy_s( fname1, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DLoadGroundBMP : filename1 error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	char filename[MAX_PATH];
	if( tempname1 ){
		strcpy_s( filename, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DImportMQOFileAsGround : filename error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	char texdir[MAX_PATH];
	if( tempname1 ){
		strcpy_s( texdir, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSigLoadFromBufThread : texdir error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	char materialname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( materialname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetToonEdge0Color : materialname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	char motname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( motname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DSetMotionName : motname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	char matname[MAX_PATH];
	if( tempname1 ){
		strcpy_s( matname, MAX_PATH, tempname1 );
	}else{
		DbgOut( "E3DGetMaterialNoByName : matname error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
