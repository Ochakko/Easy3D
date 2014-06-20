#regcmd "_hsp3cmdinit@4","e3dhsp3.dll"
#cmd E3DInit $000
#cmd E3DBye $001
#cmd E3DSigLoad $002
#cmd E3DCreateSwapChain $003
#cmd E3DDestroySwapChain $004
#cmd E3DRender $005
#cmd E3DPresent $006
#cmd E3DBeginScene $007
#cmd E3DEndScene $008
#cmd E3DCreateBG $009
#cmd E3DSetBGU $00A
#cmd E3DSetBGV $00B
#cmd E3DDestroyBG $00C
#cmd E3DAddMotion $00D
#cmd E3DSetMotionKind $00E
#cmd E3DGetMotionKind $00F
#cmd E3DSetNewPose $010
#cmd E3DSetMotionStep $011
#cmd E3DSetPos $018
#cmd E3DGetPos $019
#cmd E3DSetDir $01A
#cmd E3DRotateInit $01B
#cmd E3DRotateX $01C
#cmd E3DRotateY $01D
#cmd E3DRotateZ $01E
#cmd E3DTwist $01F
#cmd E3DPosForward $020
#cmd E3DCloseTo $021
#cmd E3DDirToTheOtherXZ $022
#cmd E3DDirToTheOther $023
#cmd E3DSeparateFrom $024
#cmd E3DGetCameraPos $025
#cmd E3DSetCameraPos $026
#cmd E3DGetCameraDeg $027
#cmd E3DSetCameraDeg $028
#cmd E3DSetCameraTarget $029
#cmd E3DChkInView $02A
#cmd E3DEnableDbgFile $02B
#cmd E3DSetProjection $02C
#cmd E3DGetKeyboardState $02D

#cmd E3DGetKeyboardCnt $162
#cmd E3DResetKeyboardCnt $163


#cmd E3DChkConflict $012 ---> E3DChkConflictAABB
#cmd E3DChkConflict2 $013 ---> E3DChkConflictOBB
#cmd E3DChkConflict3 $158 ---> E3DChkConflictOBBArray
#cmd E3DChkConfBillboard $04E
#cmd E3DChkConfBillboard2 $04D ---> E3DChkConfBillboardBS
#cmd E3DChkConfBySphere $04F ---> E3DChkConflictBS
#cmd E3DChkConfBySphere2 $050 ---> E3DChkConflictBSPart
#cmd E3DChkConfBySphere3 $159 ---> E3DChkConflictBSPartArray
#cmd E3DChkConfGround $057
#cmd E3DChkConfGround2 $058 ---> E3DChkConfGroundVec
#cmd E3DChkConfGroundPart $152
#cmd E3DChkConfGroundPart2 $153 ---> E3DChkConfGroundVecPart
#cmd E3DChkConfWall $06B
#cmd E3DChkConfWall2 $06C ---> E3DChkConfWallVec
#cmd E3DChkConfWall3 $17C ---> E3DChkConfWallPartArray
#cmd E3DChkConfLineAndFace $0F9
#cmd E3DChkConfParticle $180


#cmd E3DDrawText $036
#cmd E3DDrawBigText $037

#cmd E3DCreateFont $0D3
#cmd E3DDrawTextByFontID $0D4
#cmd E3DDrawTextByFontIDWithCnt $0D5
#cmd E3DDrawTextByFontIDWithCnt2 $0D6 ---> E3DDrawTextByFontIDWithCntEach
#cmd E3DGetCharacterNum $0D7
#cmd E3DDestroyFont $0D8


#cmd E3DGetFPS $03A
#cmd E3DWaitByFPS $03B
#cmd E3DCreateLight $03C
#cmd E3DSetDirectionalLight $03D
#cmd E3DSetPointLight $03E
#cmd E3DDestroyLight $040
#cmd E3DClearZ $041
#cmd E3DDestroyHandlerSet $042
#cmd E3DSetMotionFrameNo $044

#cmd E3DCreateSprite $045
#cmd E3DBeginSprite $046
#cmd E3DEndSprite $047
#cmd E3DRenderSprite $048
#cmd E3DGetSpriteSize $049
#cmd E3DSetSpriteRotation $04A
#cmd E3DSetSpriteARGB $04B
#cmd E3DSetSpriteUV $1B8
#cmd E3DDestroySprite $04C
#cmd E3DCreateProgressBar $051
#cmd E3DSetProgressBar $052
#cmd E3DDestroyProgressBar $053


#cmd E3DLoadGroundBMP $054
#cmd E3DLoadMQOFileAsGround $064
#cmd E3DLoadSigFileAsGround $181

#cmd E3DCameraPosForward $055
#cmd E3DSetBeforePos $056
#cmd E3DGetPartNoByName $059
#cmd E3DGetVertNumOfPart $05A
#cmd E3DGetVertPos $05B
#cmd E3DSetVertPos $05C
#cmd E3DSetVertPosBatch $1C5

#cmd E3DSetPosOnGround $05D
#cmd E3DSetPosOnGroundPart $05E
#cmd E3DCreateBillboard $05F
#cmd E3DRenderBillboard $060
#cmd E3DSetBillboardPos $061
#cmd E3DSetBillboardOnGround $062
#cmd E3DGetBillboardInfo $066
#cmd E3DGetNearBillboard $067
#cmd E3DRotateBillboard $11D
#cmd E3DSetBillboardUV $11E
#cmd E3DDestroyBillboard $063
#cmd E3DDestroyAllBillboard $095

#cmd E3DSaveMQOFile $065
#cmd E3DGetInvisibleFlag $068
#cmd E3DSetInvisibleFlag $069
#cmd E3DSetMovableArea $06A
#cmd E3DVec3Normalize $06D
#cmd E3DVec2CCW $06E
#cmd E3DVec3RotateY $06F
#cmd E3DLoadMQOFileAsMovableArea $070
#cmd E3DLoadSound $071
#cmd E3DPlaySound $072
#cmd E3DStopSound $073
#cmd E3DSetSoundLoop $074
#cmd E3DSetSoundVolume $075
#cmd E3DSetSoundTempo $076
#cmd E3DDestroySound $077
#cmd E3DGetSoundVolume $079
#cmd E3DSet3DSoundListener $07B
#cmd E3DSet3DSoundListenerMovement $07C
#cmd E3DSet3DSoundDistance $07D
#cmd E3DSet3DSoundMovement $07E



#cmd E3DCreateNaviLine $07F
#cmd E3DDestroyNaviLine $080
#cmd E3DAddNaviPoint $081
#cmd E3DRemoveNaviPoint $082
#cmd E3DGetNaviPointPos $083
#cmd E3DSetNaviPointPos $084
#cmd E3DGetNaviPointOwnerID $085
#cmd E3DSetNaviPointOwnerID $086
#cmd E3DGetNextNaviPoint $087
#cmd E3DGetPrevNaviPoint $088
#cmd E3DGetNearestNaviPoint $089
#cmd E3DFillUpNaviLine $08A
#cmd E3DSetNaviLineOnGround $08B
#cmd E3DControlByNaviLine $08C

#cmd E3DSetDirQ $08D
#cmd E3DGetDirQ $08E
#cmd E3DSetWallOnGround $08F
#cmd E3DCreateNaviPointClearFlag $090
#cmd E3DDestroyNaviPointClearFlag $091
#cmd E3DInitNaviPointClearFlag $092
#cmd E3DSetNaviPointClearFlag $093
#cmd E3DGetOrder $094

#cmd E3DSetValidFlag $096
#cmd E3DSetDiffuse $097 ---> E3DSetBillboardDiffuse
#cmd E3DSetSpecular $098 ---> E3DSetBillboardSpecular
#cmd E3DSetAmbient $099 ---> E3DSetBillboardAmbient
#cmd E3DSetEmissive $147 ---> E3DSetBillboardEmissive
#cmd E3DSetSpecularPower $148 ---> E3DSetBillboardSpecularPower
#cmd E3DSetAlpha2 $017 ---> E3DSetBillboardAlpha

#cmd E3DGetDiffuse $0DD --> E3DGetBillboardDiffuse
#cmd E3DGetAmbient $0DE --> E3DGetBillboardAmbient
#cmd E3DGetSpecular $0DF --> E3DGetBillboardSpecular
#cmd E3DGetEmissive $149 --> E3DGetBillboardEmissive
#cmd E3DGetSpecularPower $14A --> E3DGetBillboardSpecularPower
#cmd E3DGetAlpha $0E0 --> E3DGetBillboardAlpha


#cmd E3DSetBlendingMode $09A
#cmd E3DSetRenderState $09B
#cmd E3DSetScale $09C
#cmd E3DGetScreenPos3 $167 --> E3DGetScreenPos3F
#cmd E3DGetScreenPos2 $09E --> E3DGetScreenPos3FVec
#cmd E3DConvScreenTo3D $116

#cmd E3DCreateQ $09F
#cmd E3DDestroyQ $0A0
#cmd E3DInitQ $0A1
#cmd E3DSetQAxisAndDeg $0A2
#cmd E3DGetQAxisAndDeg $0A3
#cmd E3DRotateQX $0A4
#cmd E3DRotateQY $0A5
#cmd E3DRotateQZ $0A6
#cmd E3DMultQ $0A7
#cmd E3DNormalizeQ $0A8
#cmd E3DCopyQ $0A9
#cmd E3DQtoEuler $1B0


#cmd E3DGetBoneNoByName $0AA
#cmd E3DGetNextMP $0AB
#cmd E3DGetMPInfo $0B4
#cmd E3DSetMPInfo $0AC
#cmd E3DIsExistMP $0AD --> E3DExistMP
#cmd E3DGetMotionFrameLength $0AE
#cmd E3DSetMotionFrameLength $0AF
#cmd E3DAddMP $0B0
#cmd E3DDeleteMP $0B1
#cmd E3DFillUpMotion $0B2
#cmd E3DCopyMotionFrame $0B3


#cmd E3DGetDirQ2 $0B5
#cmd E3DSetDirQ2 $0B6
#cmd E3DLookAtQ $0B7
#cmd E3DMultQVec $0B8
#cmd E3DTwistQ $0B9
#cmd E3DInitTwistQ $0BA
#cmd E3DGetTwistQ $0BB
#cmd E3DRotateQLocalX $0BC
#cmd E3DRotateQLocalY $0BD
#cmd E3DRotateQLocalZ $0BE

#cmd E3DGetBonePos $0BF
#cmd E3DCreateLine $0C0
#cmd E3DDestroyLine $0C1
#cmd E3DSetLineColor $0C2
#cmd E3DAddPoint2Line $0C3
#cmd E3DDeletePointOfLine $0C4
#cmd E3DSetPointPosOfLine $0C5
#cmd E3DGetPointPosOfLine $0C6
#cmd E3DGetNextPointOfLine $0C7
#cmd E3DGetPrevPointOfLine $0C8

#cmd E3DWriteDisplay2BMP $0C9
#cmd E3DCreateAVIFile $0CA
#cmd E3DWriteData2AVIFile $0CB
#cmd E3DCompleteAVIFile $0CC

#cmd E3DCameraLookAt $0CD
#cmd E3DCameraOnNaviLine $0CE
#cmd E3DCameraDirUp $0CF
#cmd E3DCameraDirDown $0D0
#cmd E3DCameraDirRight $0D1
#cmd E3DCameraDirLeft $0D2
#cmd E3DSlerpQ $0D9
#cmd E3DSquadQ $0DA
#cmd E3DSplineVec $0DB
#cmd E3DDbgOut $0DC

#cmd E3DSaveQuaFile $0E1
#cmd E3DSaveSigFile $0E2
#cmd E3DPickVert $0E4
#cmd E3DGetOrgVertNo $0E6
#cmd E3DChkIM2Status $0E7
#cmd E3DLoadIM2File $0E8
#cmd E3DSaveIM2File $0E9
#cmd E3DGetJointNum $0EA
#cmd E3DGetJointInfo $0EB
#cmd E3DGetFirstJointNo $0EC
#cmd E3DGetDispObjNum $0ED
#cmd E3DGetDispObjInfo $0EE

#cmd E3DEnableTexture $0EF
#cmd E3DJointAddToTree $0F0
#cmd E3DJointRemoveFromParent $0F1
#cmd E3DJointRemake $0F2
#cmd E3DSigImport $0F3
#cmd E3DSigLoadFromBuf $0F4
#cmd E3DSigImportFromBuf $0F5
#cmd E3DAddMotionFromBuf $0F6
#cmd E3DCheckFullScreenParams $0F7
#cmd E3DGetMaxMultiSampleNum $0F8

#cmd E3DPickFace $0FA
#cmd E3DPickFace2 $17B --> E3DPickFacePartArray
#cmd E3DGetBBox $0FB
#cmd E3DGetVertNoOfFace $0FC
#cmd E3DGetSamePosVert $0FD
#cmd E3DRdtscStart $0FE --> E3DRtscStart
#cmd E3DRdtscStop $0FF --> E3DRtscStop

#cmd E3DSaveSig2Buf $100
#cmd E3DSaveQua2Buf $101
#cmd E3DCameraShiftLeft $102
#cmd E3DCameraShiftRight $103
#cmd E3DCameraShiftUp $104
#cmd E3DCameraShiftDown $105
#cmd E3DGetCameraQ $106
#cmd E3DInvQ $107
#cmd E3DSetCameraTwist $108


#cmd E3DIKRotateBeta $109
#cmd E3DIKRotateBeta2D $10A
#cmd E3DGetMotionType $10B
#cmd E3DSetMotionType $10C
#cmd E3DGetIKTransFlag $10D
#cmd E3DSetIKTransFlag $10E
#cmd E3DDestroyAllMotion $10F
#cmd E3DGetUserInt1OfPart $110
#cmd E3DSetUserInt1OfPart $111
#cmd E3DGetBSphere $112
#cmd E3DGetChildJoint $113

#cmd E3DDestroyMotionFrame $114
#cmd E3DGetKeyFrameNo $115
#cmd E3DSetUV $118
#cmd E3DGetUV $119
#cmd E3DPickBone $11A
#cmd E3DShiftBoneTree2D $11B

#cmd E3DSetDispSwitch2 $1AF ---> E3DSetDispSwtich
#cmd E3DGetDispSwitch2 $1B3 ---> E3DGetDispSwtich


#cmd E3DCreateTexture $11F
#cmd E3DGetTextureInfo $120
#cmd E3DGetTextureFromDispObj $122
#cmd E3DSetTextureToDispObj $123
#cmd E3DDestroyTexture $126
#cmd E3DSetLightSpecular $128
#cmd E3DCreateTextureFromBuf $12D
#cmd E3DLoadSoundFromBuf $12E
#cmd E3DTwistBone $12F
#cmd E3DSetStartPointOfSound $130

#cmd E3DGetBoneQ $131
#cmd E3DSetBoneQ $132

#cmd E3DIsSoundPlaying $133
#cmd E3DIKTranslate $134
#cmd E3DSetUVTile $135
#cmd E3DImportMQOFileAsGround $136
#cmd E3DLoadMQOFileAsMovableAreaFromBuf $137
#cmd E3DChkThreadWorking $138

#cmd E3DLoadMQOFileAsGroundThread $139
#cmd E3DSigLoadThread $13A
#cmd E3DSigLoadFromBufThread $13B
#cmd E3DLoadMQOFileAsMovableAreaThread $13C
#cmd E3DLoadMQOFileAsMovableAreaFromBufThread $13D
#cmd E3DLoadGroundBMPThread $13E

#cmd E3DAddMotionThread $141
#cmd E3DAddMotionFromBufThread $142
#cmd E3DGetShaderType $143
#cmd E3DSetShaderType $144
#cmd E3DGetInfElemNum $14B
#cmd E3DGetInfElem $14C
#cmd E3DSetInfElem $14D
#cmd E3DDeleteInfElem $14E
#cmd E3DSetInfElemDefault $14F
#cmd E3DNormalizeInfElem $150
#cmd E3DGetMidiMusicTime $154
#cmd E3DGetReferenceTime $15F
#cmd E3DSetNextMotionFrameNo $155

#cmd E3DSetLinearFogParams $156
#cmd E3DSetToon0Params $157


#cmd E3DSetMovableAreaThread $15A

#cmd E3DEncodeBeta $15C
#cmd E3DDecodeBeta $15D
#cmd E3DChkVertexShader $15E


#cmd E3DLoadMOAFile $160
#cmd E3DSetNewPoseByMOA $161
#cmd E3DGetMotionFrameNo $164

#cmd E3DGetMoaInfo $165




#cmd E3DGetNextMotionFrameNo $166

#cmd E3DEnableToonEdge $168
#cmd E3DSetToonEdge0Color $169
#cmd E3DSetToonEdge0Width $16A
#cmd E3DGetToonEdge0Color $16B
#cmd E3DGetToonEdge0Width $16C

#cmd E3DCreateParticle $16D
#cmd E3DSetParticlePos $16E
#cmd E3DSetParticleGravity $16F
#cmd E3DSetParticleLife $170
#cmd E3DSetParticleEmitNum $171
#cmd E3DSetParticleVel0 $172
#cmd E3DRenderParticle $173
#cmd E3DSetParticleRotation $174
#cmd E3DSetParticleDiffuse $175
#cmd E3DDestroyParticle $176
#cmd E3DSetParticleAlpha $177
#cmd E3DSetParticleUVTile $178
#cmd E3DInitParticle $179



#cmd E3DGetMotionIDByName $17D
#cmd E3DGetMotionNum $17F
#cmd E3DLoadSigFileAsGroundFromBuf $182
#cmd E3DGetCenterPos $183
#cmd E3DGetFaceNum $184
#cmd E3DGetFaceNormal $185
#cmd E3DCreateEmptyMotion $186
#cmd E3DSetTextureMinMagFilter $187
#cmd E3DGetMotionName $188
#cmd E3DSetMotionName $189


#cmd E3DGetMaterialNoByName $18A
#cmd E3DGetMaterialAlpha $18B
#cmd E3DGetMaterialDiffuse $18C
#cmd E3DGetMaterialSpecular $18D
#cmd E3DGetMaterialAmbient $18E
#cmd E3DGetMaterialEmissive $18F
#cmd E3DSetMaterialAlpha $190
#cmd E3DSetMaterialDiffuse $191
#cmd E3DSetMaterialSpecular $192
#cmd E3DSetMaterialAmbient $193
#cmd E3DSetMaterialEmissive $194
#cmd E3DGetTextureFromMaterial $195
#cmd E3DSetTextureToMaterial $196
#cmd E3DGetMaterialPower $197
#cmd E3DSetMaterialPower $198
#cmd E3DGetMaterialBlendingMode $199
#cmd E3DSetMaterialBlendingMode $19A

#cmd E3DGetMaterialNo $19B

#cmd E3DSetMotionKindML $19C
#cmd E3DSetMotionFrameNoML $19D
#cmd E3DSetNewPoseML $19E
#cmd E3DSetMOAEventNoML $19F
#cmd E3DSetNextMotionFrameNoML $1A0
#cmd E3DGetMotionFrameNoML $1A1
#cmd E3DGetNextMotionFrameNoML $1A2

#cmd E3DGetCurrentBonePos $1A3
#cmd E3DGetCurrentBoneQ $1A4



#cmd E3DChkBumpMapEnable $1A5
#cmd E3DEnableBumpMap $1A6
#cmd E3DConvHeight2NormalMap $1A7
#cmd E3DSetBumpMapToMaterial $1A8

#cmd E3DGetMOATrunkInfo $1AB
#cmd E3DGetMOABranchInfo $1AC
#cmd E3DSetMOABranchFrame1 $1AD
#cmd E3DSetMOABranchFrame2 $1AE


#cmd E3DEnablePhongEdge $1B1
#cmd E3DSetPhongEdge0Params $1B2

#cmd E3DFreeThread $1B4
#cmd E3DLoadSigFileAsGroundThread $1B5
#cmd E3DLoadSigFileAsGroundFromBufThread $1B6
#cmd E3DSetLightIdOfBumpMap $1B7




#cmd E3DCreateRenderTargetTexture $1B9
#cmd E3DDestroyRenderTargetTexture $1BA
#cmd E3DSetDSFillUpMode $1BB
#cmd E3DSetTexFillUpMode $1BC

#cmd E3DSetShadowMapCamera $1BD
#cmd E3DSetShadowMapProjOrtho $1BE



#cmd E3DRenderWithShadow $1BF
#cmd E3DSetShadowBias $1C0
#cmd E3DCheckRTFormat $1C1
#cmd E3DChkShadowEnable $1C2
#cmd E3DEnableShadow $1C3

#cmd E3DRenderBatch $1C4

#cmd E3DSetShadowDarkness $1C6
#cmd E3DSetShadowMapLightDir $1C7
#cmd E3DRenderBatchMode $1C8
#cmd E3DGlobalToLocal $1C9
#cmd E3DLocalToGlobal $1CA
#cmd E3DSetShadowMapMode $1CC

#cmd E3DLoadTextureFromBuf $1CD

#cmd E3DSetShadowMapInterFlag $1CF
#cmd E3DTransformBillboard $1D0
#cmd E3DSetMaterialAlphaTest $1D1





////////////////////////////////////////
//////   îpé~ñΩóﬂ
#cmd E3DCreateAfterImage $014
#cmd E3DDestroyAfterImage $015

#cmd E3DSetAlpha $016
#cmd E3DSetSpotLight $03F
#cmd E3DSetDispSwitch $043

#cmd E3DCreateSpriteFromBMSCR $15B
#cmd E3DLoadSpriteFromBMSCR $1CE


#cmd E3DCos $02E
#cmd E3DSin $02F
#cmd E3DACos $030
#cmd E3DDot $031
#cmd E3DCross $032
#cmd E3DAtan $033
#cmd E3DAtan2 $034
#cmd E3DSqrt $035
#cmd E3DRand $038
#cmd E3DSeed $039

#cmd E3DSetSoundFrequency $078
#cmd E3DGetSoundFrequency $07A

#cmd E3DGetScreenPos $09D
#cmd E3DSetMipMapParams $0E3
#cmd E3DGetCullingFlag $0E5

#cmd E3DVec3Length $117
#cmd E3DGetDispSwitch $11C

#cmd E3DSetLightAmbient $127
#cmd E3DSaveGndFile $12A

#cmd E3DLoadGndFile $12B
#cmd E3DLoadGndFileFromBuf $12C

#cmd E3DLoadGndFileThread $13F
#cmd E3DLoadGndFileFromBufThread $140

#cmd E3DSetLightBlendMode $145
#cmd E3DGetLightBlendMode $146
#cmd E3DGetVisiblePolygonNum $151

#cmd E3DDot2 $17E

#cmd E3DDumpQ $1A9
#cmd E3DSetDbgQ $1AA

#cmd E3DSetVertPosBatchAOrder $1CB

////////////////////////////////////////////
///// çƒé¿ëï
#cmd E3DCopyTextureToTexture $121
#cmd E3DRasterScrollTexture $124
#cmd E3DCopyTextureToBackBuffer $125
#cmd E3DInvColTexture $129
#cmd E3DClone $17A
 

