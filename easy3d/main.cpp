

////////////////////////
/// plugin func

static int E3DInit();//0x00
static int E3DBye();//0x01
static int E3DSigLoad();//0x02
static int E3DCreateSwapChain();//0x03
static int E3DDestroySwapChain();//0x04
static int E3DRender();//0x05
static int E3DPresent();//0x06
static int E3DBeginScene();//0x07
static int E3DEndScene();//0x08
static int E3DCreateBG();//0x09
static int E3DSetBGU();//0x0A
static int E3DSetBGV();//0x0B
static int E3DDestroyBG();//0x0C
static int E3DAddMotion();//0x0D
static int E3DSetMotionKind();//0x0E
static int E3DGetMotionKind();//0x0F
static int E3DSetNewPose();//0x10
static int E3DSetMotionStep();//0x11
static int E3DChkConflict();//0x12
static int E3DChkConflict2();//0x13

static int E3DCreateAfterImage();//0x14
static int E3DDestroyAfterImage();//0x15
static int E3DSetAlpha();//0x16
static int E3DSetAlpha2();//0x17
static int E3DSetPos();//0x18
static int E3DGetPos();//0x19
static int E3DSetDir();//0x1A
static int E3DRotateInit();//0x1B
static int E3DRotateX();//0x1C
static int E3DRotateY();//0x1D
static int E3DRotateZ();//0x1E
static int E3DTwist();//0x1F
static int E3DPosForward();//0x20
static int E3DCloseTo();//0x21
static int E3DDirToTheOtherXZ();//0x22
static int E3DDirToTheOther();//0x23
static int E3DSeparateFrom();//0x24
static int E3DGetCameraPos();//0x25
static int E3DSetCameraPos();//0x26
static int E3DGetCameraDeg();//0x27
static int E3DSetCameraDeg();//0x28
static int E3DSetCameraTarget();//0x29
static int E3DChkInView();//0x2A
static int E3DEnableDbgFile();//0x2B
static int E3DSetProjection();//0x2C
static int E3DGetKeyboardState();//0x2D

static int E3DCos();//0x2E
static int E3DSin();//0x2F
static int E3DACos();//0x30
static int E3DDot();//0x31
static int E3DCross();//0x32
static int E3DAtan();//0x33
static int E3DAtan2();//0x34
static int E3DSqrt();//0x35
static int E3DDrawText();//0x36
static int E3DDrawBigText();//0x37
static int E3DRand();//0x38
static int E3DSeed();//0x39
static int E3DGetFPS();//0x3A
static int E3DWaitbyFPS();//0x3B
static int E3DCreateLight();//0x3C
static int E3DSetDirectionalLight();//0x3D
static int E3DSetPointLight();//0x3E
static int E3DSetSpotLight();//0x3F
static int E3DDestroyLight();//0x40
static int E3DClearZ();//0x41
static int E3DDestroyHandlerSet();//0x42
static int E3DSetDispSwitch();//0x43
static int E3DSetMotionFrameNo();//0x44
static int E3DCreateSprite();//0x45
static int E3DBeginSprite();//0x46
static int E3DEndSprite();//0x47
static int E3DRenderSprite();//0x48
static int E3DGetSpriteSize();//0x49
static int E3DSetSpriteRotation();//0x4A
static int E3DSetSpriteARGB();//0x4B
static int E3DDestroySprite();//0x4C
static int E3DChkConfBillboard2();//0x4D
static int E3DChkConfBillboard();//0x4E
static int E3DChkConfBySphere();//0x4F
static int E3DChkConfBySphere2();//0x50
static int E3DCreateProgressBar();//0x051
static int E3DSetProgressBar();//0x052
static int E3DDestroyProgressBar();//0x053
static int E3DLoadGroundBMP();//0x054
static int E3DCameraPosForward();//0x055
static int E3DSetBeforePos();//0x056
static int E3DChkConfGround();//0x057
static int E3DChkConfGround2();//0x058
static int E3DGetPartNoByName();//0x059
static int E3DGetVertNumOfPart();//0x05A
static int E3DGetVertPos();//0x05B
static int E3DSetVertPos();//0x05C
static int E3DSetPosOnGround();//0x05D
static int E3DSetPosOnGroundPart();//0x05E
static int E3DCreateBillboard();//0x05F
static int E3DRenderBillboard();//0x060
static int E3DSetBillboardPos();//0x061
static int E3DSetBillboardOnGround();//0x062
static int E3DDestroyBillboard();//0x063

static int E3DLoadMQOFileAsGround();//0x064
static int E3DSaveMQOFile();//0x065
static int E3DGetBillboardInfo();//0x066
static int E3DGetNearBillboard();//0x067
static int E3DGetInvisibleFlag();//0x068
static int E3DSetInvisibleFlag();//0x069
static int E3DSetMovableArea();//0x06A
static int E3DChkConfWall();//0x06B
static int E3DChkConfWall2();//0x06C
static int E3DVec3Normalize();//0x06D
static int E3DVec2CCW();//0x06E
static int E3DVec3RotateY();//0x06F
static int E3DLoadMQOFileAsMovableArea();//0x070
static int E3DLoadSound();//0x071
static int E3DPlaySound();//0x072
static int E3DStopSound();//0x073
static int E3DSetSoundLoop();//0x074
static int E3DSetSoundVolume();//0x075
static int E3DSetSoundTempo();//0x076
static int E3DDestroySound();//0x077
static int E3DSetSoundFrequency();//0x078
static int E3DGetSoundVolume();//0x079
static int E3DGetSoundFrequency();//0x07A
static int E3DSet3DSoundListener();//0x07B
static int E3DSet3DSoundListenerMovement();//0x07C
static int E3DSet3DSoundDistance();//0x07D
static int E3DSet3DSoundMovement();//0x07E
static int E3DCreateNaviLine();//0x07F
static int E3DDestroyNaviLine();//0x080
static int E3DAddNaviPoint();//0x081
static int E3DRemoveNaviPoint();//0x082
static int E3DGetNaviPointPos();//0x083
static int E3DSetNaviPointPos();//0x084
static int E3DGetNaviPointOwnerID();//0x085
static int E3DSetNaviPointOwnerID();//0x086
static int E3DGetNextNaviPoint();//0x087
static int E3DGetPrevNaviPoint();//0x088
static int E3DGetNearestNaviPoint();//0x089
static int E3DFillUpNaviLine();//0x08A
static int E3DSetNaviLineOnGround();//0x08B
static int E3DControlByNaviLine();//0x08C
static int E3DSetDirQ();//0x08D
static int E3DGetDirQ();//0x08E
static int E3DSetWallOnGround();//0x08F
static int E3DCreateNaviPointClearFlag();//0x090
static int E3DDestroyNaviPointClearFlag();//0x091
static int E3DInitNaviPointClearFlag();//0x092
static int E3DSetNaviPointClearFlag();//0x093
static int E3DGetOrder();//0x094
static int E3DDestroyAllBillboard();//0x095
static int E3DSetValidFlag();//0x096
static int E3DSetDiffuse();//0x097
static int E3DSetSpecular();//0x098
static int E3DSetAmbient();//0x099
static int E3DSetBlendingMode();//0x09A
static int E3DSetRenderState();//0x09B
static int E3DSetScale();//0x09C
static int E3DGetScreenPos();//0x09D
static int E3DGetScreenPos2();//0x09E
static int E3DCreateQ();//0x09F
static int E3DDestroyQ();//0x0A0
static int E3DInitQ();//0x0A1
static int E3DSetQAxisAndDeg();//0x0A2
static int E3DGetQAxisAndDeg();//0x0A3
static int E3DRotateQX();//0x0A4
static int E3DRotateQY();//0x0A5
static int E3DRotateQZ();//0x0A6
static int E3DMultQ();//0x0A7
static int E3DNormalizeQ();//0x0A8
static int E3DCopyQ();//0x0A9


static int E3DGetBoneNoByName();//0x0AA
static int E3DGetNextMP();//0x0AB
static int E3DSetMPInfo();//0x0AC
static int E3DIsExistMP();//0x0AD
static int E3DGetMotionFrameLength();//0x0AE
static int E3DSetMotionFrameLength();//0x0AF
static int E3DAddMP();//0x0B0
static int E3DDeleteMP();//0x0B1
static int E3DFillUpMotion();//0x0B2
static int E3DCopyMotionFrame();//0x0B3
static int E3DGetMPInfo();//0x0B4
static int E3DGetDirQ2();//0x0B5
static int E3DSetDirQ2();//0x0B6
static int E3DLookAtQ();//0x0B7
static int E3DMultQVec();//0x0B8
static int E3DTwistQ();//0x0B9
static int E3DInitTwistQ();//0x0BA
static int E3DGetTwistQ();//0x0BB
static int E3DRotateQLocalX();//0x0BC
static int E3DRotateQLocalY();//0x0BD
static int E3DRotateQLocalZ();//0x0BE
static int E3DGetBonePos();//0x0BF
static int E3DCreateLine();//0x0C0
static int E3DDestroyLine();//0x0C1
static int E3DSetLineColor();//0x0C2
static int E3DAddPoint2Line();//0x0C3
static int E3DDeletePointOfLine();//0x0C4
static int E3DSetPointPosOfLine();//0x0C5
static int E3DGetPointPosOfLine();//0x0C6
static int E3DGetNextPointOfLine();//0x0C7
static int E3DGetPrevPointOfLine();//0x0C8
static int E3DWriteDisplay2BMP();//0x0C9
static int E3DCreateAVIFile();//0x0CA
static int E3DWriteData2AVIFile();//0x0CB
static int E3DCompleteAVIFile();//0x0CC
static int E3DCameraLookAt();//0x0CD
static int E3DCameraOnNaviLine();//0x0CE
static int E3DCameraDirUp();//0x0CF
static int E3DCameraDirDown();//0x0D0
static int E3DCameraDirRight();//0x0D1
static int E3DCameraDirLeft();//0x0D2
static int E3DCreateFont();//0x0D3
static int E3DDrawTextByFontID();//0x0D4
static int E3DDrawTextByFontIDWithCnt();//0x0D5
static int E3DDrawTextByFontIDWithCnt2();//0x0D6
static int E3DGetCharacterNum();//0x0D7
static int E3DDestroyFont();//0x0D8
static int E3DSlerpQ();//0x0D9
static int E3DSquadQ();//0x0DA
static int E3DSplineVec();//0x0DB
static int E3DDbgOut();//0x0DC
static int E3DGetDiffuse();//0x0DD
static int E3DGetAmbient();//0x0DE
static int E3DGetSpecular();//0x0DF
static int E3DGetAlpha();//0x0E0
static int E3DSaveQuaFile();//0x0E1
static int E3DSaveSigFile();//0x0E2
static int E3DSetMipMapParams();//0x0E3
static int E3DPickVert();//0x0E4
static int E3DGetCullingFlag();//0x0E5
static int E3DGetOrgVertNo();//0x0E6
static int E3DChkIM2Status();//0x0E7
static int E3DLoadIM2File();//0x0E8
static int E3DSaveIM2File();//0x0E9
static int E3DGetJointNum();//0x0EA
static int E3DGetJointInfo();//0x0EB
static int E3DGetFirstJointNo();//0x0EC
static int E3DGetDispObjNum();//0x0ED
static int E3DGetDispObjInfo();//0x0EE
static int E3DEnableTexture();//0x0EF
static int E3DJointAddToTree();//0x0F0
static int E3DJointRemoveFromParent();//0x0F1
static int E3DJointRemake();//0x0F2
static int E3DSigImport();//0x0F3
static int E3DSigLoadFromBuf();//0x0F4
static int E3DSigImportFromBuf();//0x0F5
static int E3DAddMotionFromBuf();//0x0F6
static int E3DCheckFullScreenParams();//0x0F7
static int E3DGetMaxMultiSampleNum();//0x0F8
static int E3DChkConfLineAndFace();//0x0F9
static int E3DPickFace();//0x0FA
static int E3DGetBBox();//0x0FB
static int E3DGetVertNoOfFace();//0x0FC
static int E3DGetSamePosVert();//0x0FD
static int E3DRdtscStart();//0x0FE
static int E3DRdtscStop();//0x0FF
static int E3DSaveSig2Buf();//0x100
static int E3DSaveQua2Buf();//0x101
static int E3DCameraShiftLeft();//0x102
static int E3DCameraShiftRight();//0x103
static int E3DCameraShiftUp();//0x104
static int E3DCameraShiftDown();//0x105
static int E3DGetCameraQ();//0x106
static int E3DInvQ();//0x107
static int E3DSetCameraTwist();//0x108


static int E3DIKRotateBeta();//0x109
static int E3DIKRotateBeta2D();//0x10A
static int E3DGetMotionType();//0x10B
static int E3DSetMotionType();//0x10C
static int E3DGetIKTransFlag();//0x10D
static int E3DSetIKTransFlag();//0x10E
static int E3DDestroyAllMotion();//0x10F
static int E3DGetUserInt1OfPart();//0x110
static int E3DSetUserInt1OfPart();//0x111
static int E3DGetBSphere();//0x112
static int E3DGetChildJoint();//0x113
static int E3DDestroyMotionFrame();//0x114
static int E3DGetKeyFrameNo();//0x115
static int E3DConvScreenTo3D();//0x116
static int E3DVec3Length();//0x117
static int E3DSetUV();//0x118
static int E3DGetUV();//0x119
static int E3DPickBone();//0x11A
static int E3DShiftBoneTree2D();//0x11B
static int E3DGetDispSwitch();//0x11C
static int E3DRotateBillboard();//0x11D
static int E3DSetBillboardUV();//0x11E
static int E3DCreateTexture();//0x11F
static int E3DGetTextureInfo();//0x120
static int E3DCopyTextureToTexture();//0x121
static int E3DGetTextureFromDispObj();//0x122
static int E3DSetTextureToDispObj();//0x123
static int E3DRasterScrollTexture();//0x124
static int E3DCopyTextureToBackBuffer();//0x125
static int E3DDestroyTexture();//0x126
static int E3DSetLightAmbient();//0x127
static int E3DSetLightSpecular();//0x128
static int E3DInvColTexture();//0x129
static int E3DSaveGndFile();//0x12A
static int E3DLoadGndFile();//0x12B
static int E3DLoadGndFileFromBuf();//0x12C
static int E3DCreateTextureFromBuf();//0x12D
static int E3DLoadSoundFromBuf();//0x12E
static int E3DTwistBone();//0x12F
static int E3DSetStartPointOfSound();//0x130


static int E3DGetBoneQ();//0x131
static int E3DSetBoneQ();//0x132
static int E3DIsSoundPlaying();//0x133
static int E3DIKTranslate();//0x134
static int E3DSetUVTile();//0x135
static int E3DImportMQOFileAsGround();//0x136
static int E3DLoadMQOFileAsMovableAreaFromBuf();//0x137
static int E3DChkThreadWorking();//0x138
static int E3DLoadMQOFileAsGroundThread();//0x139
static int E3DSigLoadThread();//0x13A
static int E3DSigLoadFromBufThread();//0x13B
static int E3DLoadMQOFileAsMovableAreaThread();//0x13C
static int E3DLoadMQOFileAsMovableAreaFromBufThread();//0x13D
static int E3DLoadGroundBMPThread();//0x13E
static int E3DLoadGndFileThread();//0x13F
static int E3DLoadGndFileFromBufThread();//0x140
static int E3DAddMotionThread();//0x141
static int E3DAddMotionFromBufThread();//0x142
static int E3DGetShaderType();//0x143
static int E3DSetShaderType();//0x144
static int E3DSetLightBlendMode();//0x145
static int E3DGetLightBlendMode();//0x146
static int E3DSetEmissive();//0x147
static int E3DSetSpecularPower();//0x148
static int E3DGetEmissive();//0x149
static int E3DGetSpecularPower();//0x14A
static int E3DGetInfElemNum();//0x14B
static int E3DGetInfElem();//0x14C
static int E3DSetInfElem();//0x14D
static int E3DDeleteInfElem();//0x14E
static int E3DSetInfElemDefault();//0x14F
static int E3DNormalizeInfElem();//0x150
static int E3DGetVisiblePolygonNum();//0x151
static int E3DChkConfGroundPart();//0x152
static int E3DChkConfGroundPart2();//0x153
static int E3DGetMidiMusicTime();//0x154
static int E3DSetNextMotionFrameNo();//0x155

static int E3DSetLinearFogParams();//0x156
static int E3DSetToon0Params();//0x157

static int E3DChkConflict3();//0x158
static int E3DChkConfBySphere3();//0x159

static int E3DSetMovableAreaThread();//0x15A
static int E3DCreateSpriteFromBMSCR();//0x15B

static int E3DEncodeBeta();//$15C
static int E3DDecodeBeta();//$15D
static int E3DChkVertexShader();//$15E

static int E3DGetReferenceTime();//$15F
static int E3DLoadMOAFile();//$160
static int E3DSetNewPoseByMOA();//$161

static int E3DGetKeyboardCnt();//$162
static int E3DResetKeyboardCnt();//$163
static int E3DGetMotionFrameNo();//$164

static int E3DGetMoaInfo();//$165
static int E3DGetNextMotionFrameNo();//$166
static int E3DGetScreenPos3();//$167

static int E3DEnableToonEdge();//$168
static int E3DSetToonEdge0Color();//$169
static int E3DSetToonEdge0Width();//$16A
static int E3DGetToonEdge0Color();//$16B
static int E3DGetToonEdge0Width();//$16C

static int E3DCreateParticle();//$16D
static int E3DSetParticlePos();//$16E
static int E3DSetParticleGravity();//$16F
static int E3DSetParticleLife();//$170
static int E3DSetParticleEmitNum();//$171
static int E3DSetParticleVel0();//$172
static int E3DRenderParticle();//$173
static int E3DSetParticleRotation();//$174
static int E3DSetParticleDiffuse();//$175
static int E3DDestroyParticle();//$176
static int E3DSetParticleAlpha();//$177
static int E3DSetParticleUVTile();//$178
static int E3DInitParticle();//$179

static int E3DClone();//$17A
static int E3DPickFace2();//$17B
static int E3DChkConfWall3();//$17C
static int E3DGetMotionIDByName();//$17D
static int E3DDot2();//$17E
static int E3DGetMotionNum();//$17F

static int E3DChkConfParticle();//$180

static int E3DLoadSigFileAsGround();//$181
static int E3DLoadSigFileAsGroundFromBuf();//$182
static int E3DGetCenterPos();//$183

static int E3DGetFaceNum();//$184
static int E3DGetFaceNormal();//$185
static int E3DCreateEmptyMotion();//$186
static int E3DSetTextureMinMagFilter();//$187
static int E3DGetMotionName();//$188
static int E3DSetMotionName();//$189

static int E3DGetMaterialNoByName();//$18A
static int E3DGetMaterialAlpha();//$18B
static int E3DGetMaterialDiffuse();//$18C
static int E3DGetMaterialSpecular();//$18D
static int E3DGetMaterialAmbient();//$18E
static int E3DGetMaterialEmissive();//$18F
static int E3DSetMaterialAlpha();//$190
static int E3DSetMaterialDiffuse();//$191
static int E3DSetMaterialSpecular();//$192
static int E3DSetMaterialAmbient();//$193
static int E3DSetMaterialEmissive();//$194
static int E3DGetTextureFromMaterial();//$195
static int E3DSetTextureToMaterial();//$196
static int E3DGetMaterialPower();//$197
static int E3DSetMaterialPower();//$198
static int E3DGetMaterialBlendingMode();//$199
static int E3DSetMaterialBlendingMode();//$19A

static int E3DGetMaterialNo();//$19B
static int E3DSetMotionKindML();//$19C
static int E3DSetMotionFrameNoML();//$19D
static int E3DSetNewPoseML();//$19E
static int E3DSetMOAEventNoML();//$19F
static int E3DSetNextMotionFrameNoML();//$1A0
static int E3DGetMotionFrameNoML();//$1A1
static int E3DGetNextMotionFrameNoML();//$1A2

static int E3DGetCurrentBonePos();//$1A3
static int E3DGetCurrentBoneQ();//$1A4

static int E3DChkBumpMapEnable();//$1A5
static int E3DEnableBumpMap();//$1A6
static int E3DConvHeight2NormalMap();//$1A7
static int E3DSetBumpMapToMaterial();//$1A8

static int E3DDumpQ();//$1A9
static int E3DSetDbgQ();// $1AA

static int E3DGetMOATrunkInfo();// $1AB
static int E3DGetMOABranchInfo();// $1AC
static int E3DSetMOABranchFrame1();// $1AD
static int E3DSetMOABranchFrame2();// $1AE

static int E3DSetDispSwitch2();// $1AF

static int E3DQtoEuler();// $1B0
static int E3DEnablePhongEdge();// $1B1
static int E3DSetPhongEdge0Params();// $1B2

static int E3DGetDispSwitch2();// $1B3
static int E3DFreeThread();// $1B4
static int E3DLoadSigFileAsGroundThread();// $1B5
static int E3DLoadSigFileAsGroundFromBufThread();// $1B6
static int E3DSetLightIdOfBumpMap();// $1B7
static int E3DSetSpriteUV();// $1B8

static int E3DCreateRenderTargetTexture();// $1B9
static int E3DDestroyRenderTargetTexture();// $1BA
static int E3DSetDSFillUpMode();// $1BB
static int E3DSetTexFillUpMode();// $1BC

static int E3DSetShadowMapCamera();// $1BD
static int E3DSetShadowMapProjOrtho();// $1BE
static int E3DRenderWithShadow();// $1BF
static int E3DSetShadowBias();// $1C0
static int E3DCheckRTFormat();// $1C1
static int E3DChkShadowEnable();// $1C2
static int E3DEnableShadow();// $1C3

static int E3DRenderBatch();// $1C4
static int E3DSetVertPosBatch();// $1C5

static int E3DSetShadowDarkness();// $1C6
static int E3DSetShadowMapLightDir();// $1C7
static int E3DRenderBatchMode();// $1C8
static int E3DGlobalToLocal();// $1C9
static int E3DLocalToGlobal();// $1CA
static int E3DSetVertPosBatchAOrder();// $1CB
static int E3DSetShadowMapMode();// $1CC

static int E3DLoadTextureFromBuf();// $1CD
static int E3DLoadSpriteFromBMSCR();// $1CE

///////////////////////////////
///////////////////////////////
///////////////////////////////

int GetName( char* dstname, int srcleng )
{
	int chk;
	chk = code_getprm();
	if ( chk<=PARAM_END ) {
		DbgOut( "GetName : parameter is not set error !!!\n" );
		_ASSERT( 0 );
		return 1;										// パラメーター省略時の処理
	}

	char* nameptr;
	//PVal *pvalname;
	//APTR aptrname;
	//aptrname = code_getva( &pvalname );

	//DbgOut( "GetName : mpval->mode %d\r\n", mpval->mode );

	if( mpval->flag != HSPVAR_FLAG_STR ){
		DbgOut( "GetName : this parameter is not str error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( mpval->offset == 0 ){
		nameptr = (char*)mpval->pt;
	}else{
		nameptr = *((char**)mpval->master + mpval->offset);
	}

	if( !nameptr ){
		DbgOut( "GetName : nameptr NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int leng;
	leng = (int)strlen( nameptr );
	if( leng >= srcleng ){
		DbgOut( "str size too long error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}
	strcpy( dstname, nameptr );

	return 0;
}

int E3DCos()
{
	// 1 : deg
	// 3 : retptr

	float deg;
	deg = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	float rad = deg * (float)DEG2PAI;
	
	double temp;
	temp = cosf( rad );

	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );


	return 0;

}

int E3DSin()
{
	// type $202
	// 1 : deg
	// 3 : retptr

	float deg;
	deg = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

/////////

	float rad = deg * (float)DEG2PAI;
	double temp;
	temp = sinf( rad );

	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );

	return 0;

}


int E3DACos()
{
	// type $202
	// 1 : deg
	// 3 : retptr
	float dot;
	dot = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );
////////

	if( dot > 1.0f )
		dot = 1.0f;
	else if( dot < -1.0f )
		dot = -1.0f;

	float rad;
	rad = (float)acos( dot );

	double degree;
	degree = rad * (float)PAI2DEG;

	code_setva( pvalret, aptrret, TYPE_DNUM, &degree );

	return 0;

}



int E3DDot()
{
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 8 : retptr

	float x1;
	x1 = (float)code_getdd(0.0);

	float y1;
	y1 = (float)code_getdd(0.0);

	float z1;
	z1 = (float)code_getdd(0.0);

	float x2;
	x2 = (float)code_getdd(0.0);

	float y2;
	y2 = (float)code_getdd(0.0);

	float z2;
	z2 = (float)code_getdd(0.0);


	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	D3DXVECTOR3 vec1, vec2;
	vec1.x = x1;
	vec1.y = y1;
	vec1.z = z1;

	vec2.x = x2;
	vec2.y = y2;
	vec2.z = z2;

	DXVec3Normalize( &vec1, &vec1 );
	DXVec3Normalize( &vec2, &vec2 );

	double dot;
	dot = (double)D3DXVec3Dot( &vec1, &vec2 );

	code_setva( pvalret, aptrret, TYPE_DNUM, &dot );

	return 0;
}

int E3DDot2()
{
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 8 : retptr

	float x1;
	x1 = (float)code_getdd(0.0);

	float y1;
	y1 = (float)code_getdd(0.0);

	float z1;
	z1 = (float)code_getdd(0.0);

	float x2;
	x2 = (float)code_getdd(0.0);

	float y2;
	y2 = (float)code_getdd(0.0);

	float z2;
	z2 = (float)code_getdd(0.0);


	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	D3DXVECTOR3 vec1, vec2;
	vec1.x = x1;
	vec1.y = y1;
	vec1.z = z1;

	vec2.x = x2;
	vec2.y = y2;
	vec2.z = z2;

	//DXVec3Normalize( &vec1, &vec1 );
	//DXVec3Normalize( &vec2, &vec2 );

	double dot;
	dot = (double)D3DXVec3Dot( &vec1, &vec2 );

	code_setva( pvalret, aptrret, TYPE_DNUM, &dot );

	return 0;
}



int E3DCross()
{
	// 1 : xptr1
	// 2 : yptr1
	// 3 : zptr1

	// 4 : xptr2
	// 5 : yptr2
	// 6 : zptr2
	
	// 8 : retxptr
	// 9 : retyptr
	// 10 : retzptr

	float x1;
	x1 = (float)code_getdd(0.0);

	float y1;
	y1 = (float)code_getdd(0.0);

	float z1;
	z1 = (float)code_getdd(0.0);

	float x2;
	x2 = (float)code_getdd(0.0);

	float y2;
	y2 = (float)code_getdd(0.0);

	float z2;
	z2 = (float)code_getdd(0.0);

	PVal *pvalretx;
	APTR aptrretx;
	aptrretx = code_getva( &pvalretx );

	PVal *pvalrety;
	APTR aptrrety;
	aptrrety = code_getva( &pvalrety );

	PVal *pvalretz;
	APTR aptrretz;
	aptrretz = code_getva( &pvalretz );


////////

	//！！！本来の、外積とは、異なる。！！！
	// vec1, vec2の両方に垂直で、大きさが*multptrなベクトルを計算する。

	D3DXVECTOR3 vec1, vec2;
	vec1.x = x1;
	vec1.y = y1;
	vec1.z = z1;

	vec2.x = x2;
	vec2.y = y2;
	vec2.z = z2;

	DXVec3Normalize( &vec1, &vec1 );
	DXVec3Normalize( &vec2, &vec2 );

	D3DXVECTOR3 crossvec;
	D3DXVec3Cross( &crossvec, &vec1, &vec2 );

	double dx, dy, dz;
	dx = (double)crossvec.x;
	dy = (double)crossvec.y;
	dz = (double)crossvec.z;

	code_setva( pvalretx, aptrretx, TYPE_DNUM, &dx );
	code_setva( pvalrety, aptrrety, TYPE_DNUM, &dy );
	code_setva( pvalretz, aptrretz, TYPE_DNUM, &dz );


	return 0;
}

int E3DAtan()
{
	// 1 : valptr
	// 3 : retptr

	float val;
	val = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );


////////
	float retrad;
	retrad = (float)atan( val );

	double temp;
	temp = retrad * (float)PAI2DEG;

	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );

	return 0;

}

int E3DAtan2()
{
	// 1 : yptr
	// 2 : xptr
	// 3 : retptr
	float y;
	y = (float)code_getdd(0.0);

	float x;
	x = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );
	
////////

	float retrad;
	retrad = (float)atan2( y, x );

	double temp;
	temp = retrad * (float)PAI2DEG;
	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );

	return 0;

}

int E3DSqrt()
{
	// 1 : valptr
	// 3 : retptr

	float val;
	val = (float)code_getdd(0.0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );

////////

	//E3DSqrt val, mult, ret
	//	sqrt( val ) * mult
	double temp;	
	if( val <= 0.0f ){
		temp = 0.0;
	}else{
		temp = sqrt( val );
	}
	code_setva( pvalret, aptrret, TYPE_DNUM, &temp );


	return 0;

}


int E3DRand()
{
	// 1 : rangeptr
	// 2 : retptr
	int range;
	range = code_getdi(0);

	PVal *pvalret;
	APTR aptrret;
	aptrret = code_getva( &pvalret );
////////
			
	int tempval;
	tempval = rand();

	int rangeval;
	rangeval = range;
	if( rangeval < 0 ){
		rangeval *= -1;
	}else if( rangeval == 0 ){
		rangeval = 1;
	}

	int temp;
	temp = tempval % rangeval;

	code_setva( pvalret, aptrret, TYPE_INUM, &temp );

	return 0;
}

int E3DSeed()
{
	// type $202
	// 1 : seedptr

	int seed;
	seed = code_getdi(0);


////////


	srand( (unsigned)seed );


	return 0;
}













/***
EXPORT BOOL WINAPI E3DTimeGetTime31bit( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	int curtype;
	curtype = *hei->nptype;
	int* timeptr;
	if( curtype == 4 ){
		timeptr = (int *)hei->HspFunc_prm_getv();	// パラメータ2:変数
		if( !timeptr ){
			DbgOut( "e3dhsp : TimeGetTime31bit : timeptr error !!!\n" );
			return 1;
		}
	}else{
		DbgOut( "E3DTimeGetTime31bit : timeptr parameter type error %d!!!\n", curtype );
		return 1;
	}
//
	if ( *hei->er ) return *hei->er;		// エラーチェック

///////

	DWORD curtime;
	curtime = timeGetTime();

	*timeptr = curtime & 0x7FFFFFFF;

	return 0;
}
***/

//E3DCopyTextureToTexture srctexid, desttexid
int E3DCopyTextureToTexture()
{


//1 srctexid
	int srctexid;
	srctexid = code_getdi( 0 );

//2 dsttexid
	int dsttexid;
	dsttexid = code_getdi( 0 );
////
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DCopyTextureToTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CopyTexture2Texture( srctexid, dsttexid );
	if( ret ){
		DbgOut( "E3DCopyTextureToTexture : texbank : CopyTexture2Texture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}


//E3DRasterScrollTexture srctexid, desttexid, t, param1, param1div, param2, param2div
int E3DRasterScrollTexture()
{

//1 srctexid
	int srctexid;
	srctexid = code_getdi( 0 );

//2 desttexid
	int desttexid;
	desttexid = code_getdi( 0 );

//3 t
	int t;
	t = code_getdi( 0 );

//4 param1
	float param1;
	param1 = (float)code_getdd(0.0);

//5 param2
	float param2;
	param2 = (float)code_getdd(0.0);

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DRasterScrollTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DRasterScrollTexture : GetD3DDevice error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->RasterScrollTexture( pdev, srctexid, desttexid, t, param1, param2 );
	if( ret ){
		DbgOut( "E3DRasterScrollTexture : texbank RasterScrollTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}

//E3DCopyTextureToBackBuffer texid
int E3DCopyTextureToBackBuffer()
{

//1 texid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DCopyTextureToBackBuffer : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DCopyTextureToBackBuffer : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->CopyTextureToBackBuffer( pdev, texid );
	if( ret ){
		DbgOut( "E3DCopyTextureToBackBuffer : texbank CopyTextureToBackBuffer error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}



//E3DInvColTexture texid
int E3DInvColTexture()
{

//1 lid
	int texid;
	texid = code_getdi( 0 );

////
	
	int ret;

	if( !g_texbnk ){
		DbgOut( "E3DInvColTexture : texbank NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	LPDIRECT3DDEVICE9 pdev;
	ret = g_pD3DApp->GetD3DDevice( &pdev );
	if( ret || !pdev ){
		DbgOut( "E3DInvColTexture : 3ddevice NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	ret = g_texbnk->InvColTexture( pdev, texid );
	if( ret ){
		DbgOut( "E3DInvColTexture : texbank InvColTexture error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	return 0;
}












int E3DDumpQ()//$1A9
{
	int qid;
	qid = code_getdi(0);

//////////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DDumpQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DDumpQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DDumpQ : dstqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	DbgOut( "dumpQ : qid %d, ( %1.12f %1.12f %1.12f %1.12f ) twist %f\r\n", qid, qptr->x, qptr->y, qptr->z, qptr->w, qptr->twist );

	return 0;
}

int E3DSetDbgQ()// $1AA
{
	int qid;
	qid = code_getdi(0);

//////////
	int ret;
	if( !s_qh ){
		DbgOut( "E3DSetDbgQ : qhandler not created error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CQuaternion2* qptr;
	ret = s_qh->GetQ( qid, &qptr );
	if( ret ){
		DbgOut( "E3DSetDbgQ : qh GetQ error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	if( !qptr ){
		DbgOut( "E3DSetDbgQ : dstqid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

//	qptr->x = 0.000000f;
//	qptr->y = 0.706566f;
//	qptr->z = 0.000000f;
//	qptr->w = 0.707647f;
//
//	D3DXVECTOR3 axis;
//	float rad;
//	ret = qptr->GetAxisAndRot( &axis, &rad );
//	_ASSERT( !ret );
//	float deg;
//	deg = rad * (float)PAI2DEG;
//	DbgOut( "setdbgq : axis %f %f %f, rad %f, deg %f\r\n", axis.x, axis.y, axis.z, rad, deg );

	D3DXVECTOR3 axis( 0.0f, 1.0f, 0.0f );
	qptr->SetAxisAndRot( axis, 90.0f * (float)DEG2PAI );


	return 0;
}







int E3DLoadSpriteFromBMSCR()// $1CE
{
//1
	int spriteid;
	spriteid = code_getdi(0);

//2 : wid
	int wid;
	wid = code_getdi(0);

//3 : transparent
	int transparentflag;
	transparentflag = code_getdi(0);

//4 : tpr
	int tpr = code_getdi(0);
//5 : tpg
	int tpg = code_getdi(0);
//6 : tpb
	int tpb = code_getdi(0);


////////
	int ret;


	BMSCR* bm;
	bm = (BMSCR*)exinfo->HspFunc_getbmscr( wid );
	if( !bm ){
		DbgOut( "E3DLoadSpriteFromBMSCR : bmscr NULL : window id error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	CMySprite* cursprite = 0;
	cursprite = GetSprite( spriteid );
	if( !cursprite ){
		DbgOut( "E3DLoadSpriteFromBMSCR : GetSprite spriteid error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = cursprite->SetSpriteParams2( bm->pbi, bm->pBit, transparentflag, tpr, tpg, tpb, g_miplevels, g_mipfilter );
	if( ret ){
		DbgOut( "E3DLoadSpriteFromBMSCR : newsprite SetSpriteParams error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	ret = cursprite->LoadSpriteFromBuf();
	if( ret ){
		DbgOut( "E3DLoadSpriteFromBMSCR : cursprite LoadSpriteFromBuf error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}


	return 0;
}


/***
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );
***/

/***
int E3DTestCooperativeLevel()// $1B3
{
	PVal *pvalflag;
	APTR aptrflag;
	aptrflag = code_getva( &pvalflag );

////////////

	if( !g_pD3DApp ){
		DbgOut( "E3DTestCooperative : d3dapp NULL error !!!\n" );
		_ASSERT( 0 );
		return 1;
	}

	int notrender;
	notrender = g_pD3DApp->TestCooperativeAndReset();

	int renderok;
	if( notrender == 0 ){
		renderok = 1;
	}else{
		renderok = 0;
	}

	code_setva( pvalflag, aptrflag, TYPE_INUM, &renderok );

	return 0;
}

int E3DActivateApp()// $1B4
{
	int wparam;
	wparam = code_getdi(0);
///////

	DbgOut( "E3DActivateApp : %d !!!\n", wparam );

	if( !g_pD3DApp ){
		return 0;//!!!!!!!!!!!!!!!!
	}

	if( wparam == 0 ){
		if( g_pD3DApp->m_bReady ){
			g_pD3DApp->InvalidateDeviceObjects();
			g_pD3DApp->m_pd3dDevice->Reset( &g_pD3DApp->m_d3dpp );
			//g_pD3DApp->m_bReady = 0;
			g_activateapp = 0;
		}
	}else{
		if( g_pD3DApp->m_bReady ){
			//g_pD3DApp->Resize3DEnvironment();
			//g_pD3DApp->m_bReady = 1;
			g_activateapp = 1;
		}
	}

	return 0;
}
***/
/***
	PVal *pvalhsid;
	APTR aptrhsid;
	aptrhsid = code_getva( &pvalhsid );

	code_setva( pvalhsid, aptrhsid, TYPE_INUM, &temphsid );
***/

