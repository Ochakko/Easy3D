
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Wed Apr 21 17:52:44 2004
 */
/* Compiler settings for \pgfile\RokDeBone2\RokDeBone2.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __RokDeBone2_h__
#define __RokDeBone2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISigTreeView2_FWD_DEFINED__
#define __ISigTreeView2_FWD_DEFINED__
typedef interface ISigTreeView2 ISigTreeView2;
#endif 	/* __ISigTreeView2_FWD_DEFINED__ */


#ifndef __SigTreeView2_FWD_DEFINED__
#define __SigTreeView2_FWD_DEFINED__

#ifdef __cplusplus
typedef class SigTreeView2 SigTreeView2;
#else
typedef struct SigTreeView2 SigTreeView2;
#endif /* __cplusplus */

#endif 	/* __SigTreeView2_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ISigTreeView2_INTERFACE_DEFINED__
#define __ISigTreeView2_INTERFACE_DEFINED__

/* interface ISigTreeView2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISigTreeView2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("916955BB-7928-4913-BAA5-D93330612CE0")
    ISigTreeView2 : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetParams( 
            long lpth,
            long lpsh,
            long lpmh,
            long appwnd) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDbgFile( 
            long dbgf) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Redraw( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnMove( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnUserSelchange( 
            long selno) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvalidateHandler( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowShdParamDlg( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowJointLocDlg( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NewJoint( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowInfluenceDlg( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISigTreeView2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISigTreeView2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISigTreeView2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISigTreeView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISigTreeView2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISigTreeView2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISigTreeView2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISigTreeView2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetParams )( 
            ISigTreeView2 * This,
            long lpth,
            long lpsh,
            long lpmh,
            long appwnd);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDbgFile )( 
            ISigTreeView2 * This,
            long dbgf);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Redraw )( 
            ISigTreeView2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnMove )( 
            ISigTreeView2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnUserSelchange )( 
            ISigTreeView2 * This,
            long selno);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvalidateHandler )( 
            ISigTreeView2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowShdParamDlg )( 
            ISigTreeView2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowJointLocDlg )( 
            ISigTreeView2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewJoint )( 
            ISigTreeView2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowInfluenceDlg )( 
            ISigTreeView2 * This);
        
        END_INTERFACE
    } ISigTreeView2Vtbl;

    interface ISigTreeView2
    {
        CONST_VTBL struct ISigTreeView2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISigTreeView2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISigTreeView2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISigTreeView2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISigTreeView2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISigTreeView2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISigTreeView2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISigTreeView2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISigTreeView2_SetParams(This,lpth,lpsh,lpmh,appwnd)	\
    (This)->lpVtbl -> SetParams(This,lpth,lpsh,lpmh,appwnd)

#define ISigTreeView2_SetDbgFile(This,dbgf)	\
    (This)->lpVtbl -> SetDbgFile(This,dbgf)

#define ISigTreeView2_Redraw(This)	\
    (This)->lpVtbl -> Redraw(This)

#define ISigTreeView2_OnMove(This)	\
    (This)->lpVtbl -> OnMove(This)

#define ISigTreeView2_OnUserSelchange(This,selno)	\
    (This)->lpVtbl -> OnUserSelchange(This,selno)

#define ISigTreeView2_InvalidateHandler(This)	\
    (This)->lpVtbl -> InvalidateHandler(This)

#define ISigTreeView2_ShowShdParamDlg(This)	\
    (This)->lpVtbl -> ShowShdParamDlg(This)

#define ISigTreeView2_ShowJointLocDlg(This)	\
    (This)->lpVtbl -> ShowJointLocDlg(This)

#define ISigTreeView2_NewJoint(This)	\
    (This)->lpVtbl -> NewJoint(This)

#define ISigTreeView2_ShowInfluenceDlg(This)	\
    (This)->lpVtbl -> ShowInfluenceDlg(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_SetParams_Proxy( 
    ISigTreeView2 * This,
    long lpth,
    long lpsh,
    long lpmh,
    long appwnd);


void __RPC_STUB ISigTreeView2_SetParams_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_SetDbgFile_Proxy( 
    ISigTreeView2 * This,
    long dbgf);


void __RPC_STUB ISigTreeView2_SetDbgFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_Redraw_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_Redraw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_OnMove_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_OnMove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_OnUserSelchange_Proxy( 
    ISigTreeView2 * This,
    long selno);


void __RPC_STUB ISigTreeView2_OnUserSelchange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_InvalidateHandler_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_InvalidateHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_ShowShdParamDlg_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_ShowShdParamDlg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_ShowJointLocDlg_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_ShowJointLocDlg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_NewJoint_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_NewJoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISigTreeView2_ShowInfluenceDlg_Proxy( 
    ISigTreeView2 * This);


void __RPC_STUB ISigTreeView2_ShowInfluenceDlg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISigTreeView2_INTERFACE_DEFINED__ */



#ifndef __RokDeBone2Lib_LIBRARY_DEFINED__
#define __RokDeBone2Lib_LIBRARY_DEFINED__

/* library RokDeBone2Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_RokDeBone2Lib;

EXTERN_C const CLSID CLSID_SigTreeView2;

#ifdef __cplusplus

class DECLSPEC_UUID("6CC111E2-2D14-4C9D-A25D-CD98C36D1A8F")
SigTreeView2;
#endif
#endif /* __RokDeBone2Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


