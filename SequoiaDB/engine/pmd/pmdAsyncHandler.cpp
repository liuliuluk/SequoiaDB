/*******************************************************************************


   Copyright (C) 2011-2014 SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the term of the GNU Affero General Public License, version 3,
   as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warrenty of
   MARCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program. If not, see <http://www.gnu.org/license/>.

   Source File Name = pmdAsyncHandler.cpp

   Descriptive Name = Data Management Service Header

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          1/12/2012  Xu Jianhui  Initial Draft

   Last Changed =

*******************************************************************************/

#include "core.hpp"
#include "pmdAsyncHandler.hpp"
#include "pmdAsyncSession.hpp"
#include "msgMessage.hpp"
#include "ossUtil.hpp"
#include "pdTrace.hpp"
#include "pmdTrace.hpp"
#include "pmdEnv.hpp"

namespace engine
{
   /*
      _pmdAsyncTimerHandler implement
   */
   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDTMHD, "_pmdAsyncTimerHandler::_pmdAsyncTimerHandler" )
   _pmdAsyncTimerHandler::_pmdAsyncTimerHandler( _pmdAsycSessionMgr * pSessionMgr )
   {
      PD_TRACE_ENTRY ( SDB__PMDTMHD ) ;
      _pMgrCB = NULL ;
      _pSessionMgr = pSessionMgr ;
      PD_TRACE_EXIT ( SDB__PMDTMHD ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDTMHD_DES, "_pmdAsyncTimerHandler::~_pmdAsyncTimerHandler" )
   _pmdAsyncTimerHandler::~_pmdAsyncTimerHandler()
   {
      PD_TRACE_ENTRY ( SDB__PMDTMHD_DES ) ;
      _pMgrCB = NULL ;
      _pSessionMgr = NULL ;
      PD_TRACE_EXIT ( SDB__PMDTMHD_DES ) ;
   }

   UINT64 _pmdAsyncTimerHandler::_makeTimerID( UINT32 timerID )
   {
      return ( UINT64 )timerID ;
   }

   // This function handle the timeout event
   // Since timeout event is not critical, it's OK if there's error
   // so we return void
   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDTMHD_HDTMOUT, "_pmdAsyncTimerHandler::handleTimeout" )
   void _pmdAsyncTimerHandler::handleTimeout( const UINT32 &millisec,
                                              const UINT32 &id )
   {
      PD_TRACE_ENTRY ( SDB__PMDTMHD_HDTMOUT ) ;
      UINT64 timerID = _makeTimerID ( id ) ;

      if ( _pSessionMgr->handleSessionTimeout( timerID , millisec ) != SDB_OK )
      {
         // memory will be freed in the event consumer thread
         // PMD_EDU_MEM_ALLOC will be passed into pmdEDUEvent, so that the
         // consumer knows whether to free the memory
         PMD_EVENT_MESSAGES *eventMsg = (PMD_EVENT_MESSAGES *)
               SDB_OSS_MALLOC( sizeof (PMD_EVENT_MESSAGES ) ) ;

         if ( NULL == eventMsg )
         {
            // if unable to allocate memory, let's simply return
            PD_LOG ( PDWARNING, "Failed to allocate memory for PDM "
                     "timeout Event for %d bytes",
                     sizeof (PMD_EVENT_MESSAGES ) ) ;
         }
         else
         {
            ossTimestamp ts;
            ossGetCurrentTime(ts);

            eventMsg->timeoutMsg.interval = millisec ;
            eventMsg->timeoutMsg.occurTime = ts.time ;
            eventMsg->timeoutMsg.timerID = timerID ;

            // post the timeout event of current timestamp
            _pMgrCB->postEvent( pmdEDUEvent ( PMD_EDU_EVENT_TIMEOUT,
                                              PMD_EDU_MEM_ALLOC,
                                              (void*)eventMsg ) ) ;
         }
      }
      PD_TRACE_EXIT ( SDB__PMDTMHD_HDTMOUT ) ;
   }

   /*
      _pmdAsyncMsgHandler implement
   */
   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND, "_pmdAsyncMsgHandler::_pmdAsyncMsgHandler" )
   _pmdAsyncMsgHandler::_pmdAsyncMsgHandler( _pmdAsycSessionMgr *pSessionMgr )
   {
      PD_TRACE_ENTRY ( SDB__PMDMSGHND ) ;
      _pSessionMgr   = pSessionMgr ;
      _pMgrEDUCB     = NULL ;
      PD_TRACE_EXIT ( SDB__PMDMSGHND ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_DESC, "_pmdAsyncMsgHandler::~_pmdAsyncMsgHandler" )
   _pmdAsyncMsgHandler::~_pmdAsyncMsgHandler()
   {
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_DESC ) ;
      _pSessionMgr   = NULL ;
      _pMgrEDUCB     = NULL ;
      PD_TRACE_EXIT ( SDB__PMDMSGHND_DESC ) ;
   }

   // copy content from msg and return the buffer
   // It's caller's responsibility to free the memory
   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_CPMSG, "_pmdAsyncMsgHandler::_copyMsg" )
   void * _pmdAsyncMsgHandler::_copyMsg ( const CHAR* msg, UINT32 length )
   {
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_CPMSG );
      // memory will be freed by the caller
      // TODO: OSS malloc will be replaced by session based malloc in the future
      CHAR *pBuffer = (CHAR * )SDB_OSS_MALLOC ( length ) ;
      if ( pBuffer )
      {
         ossMemcpy( pBuffer, msg, length ) ;
      }

      PD_TRACE_EXIT ( SDB__PMDMSGHND_CPMSG );
      return pBuffer ;
   }

   // This function will not be used concurrently, so we don't need to latch it
   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_HNDMSG, "_pmdAsyncMsgHandler::handleMsg" )
   INT32 _pmdAsyncMsgHandler::handleMsg( const NET_HANDLE & handle,
                                         const _MsgHeader * header,
                                         const CHAR * msg)
   {
      //If TID not Zero, implicate external business require form client
      //or repl sync messages
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_HNDMSG ) ;

      if ( (UINT32)MSG_SYSTEM_INFO_LEN == (UINT32)header->messageLength )
      {
         rc = _handleSysInfo( handle, header, msg ) ;
      }
      else if ( header->TID != 0 )
      {
         rc = _handleSessionMsg ( handle, header, msg ) ;
      }
      //Other msg will push to cb queue
      else
      {
         rc = _handleMainMsg( handle, header, msg ) ;
      }
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to handle message, rc = %d",
                  rc ) ;
         goto error ;
      }
   done :
      PD_TRACE_EXITRC ( SDB__PMDMSGHND_HNDMSG, rc ) ;
      return rc ;
   error :
      /// when this error, net will close the connect
      rc = SDB_NET_BROKEN_MSG ;
      goto done ;
   }

   INT32 _pmdAsyncMsgHandler::_handleSysInfo( const NET_HANDLE & handle,
                                              const _MsgHeader * header,
                                              const CHAR * msg )
   {
      INT32 rc = SDB_OK ;
      MsgSysInfoReply reply ;
      MsgSysInfoReply *pReply = &reply ;
      INT32 replySize = sizeof(reply) ;

      rc = msgBuildSysInfoReply ( (CHAR**)&pReply, &replySize ) ;
      PD_RC_CHECK ( rc, PDERROR, "Failed to build sys info reply, "
                    "rc = %d", rc ) ;

      rc = _pSessionMgr->getRouteAgent()->syncSendRaw( handle,
                                                       (const CHAR *)pReply,
                                                       (UINT32)replySize ) ;

   done:
      return rc ;
   error:
      goto done ;
   }

   // This function will not be used concurrently, so we don't need to latch it
   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_HNDCLOSE, "_pmdAsyncMsgHandler::handleClose" )
   void _pmdAsyncMsgHandler::handleClose ( const NET_HANDLE & handle,
                                           _MsgRouteID id )
   {
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_HNDCLOSE ) ;
      PD_LOG ( PDINFO, "connection[handle:%d] closed", handle ) ;
      _pSessionMgr->handleSessionClose( handle ) ;
      PD_TRACE_EXIT ( SDB__PMDMSGHND_HNDCLOSE ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_ONSTOP, "_pmdAsyncMsgHandler::onStop" )
   void _pmdAsyncMsgHandler::onStop()
   {
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_ONSTOP ) ;
      _pSessionMgr->handleStop() ;
      PD_TRACE_EXIT ( SDB__PMDMSGHND_ONSTOP ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_HNDSNMSG, "_pmdAsyncMsgHandler::_handleSessionMsg" )
   INT32 _pmdAsyncMsgHandler::_handleSessionMsg ( const NET_HANDLE &handle,
                                                  const _MsgHeader *header,
                                                  const CHAR *msg )
   {
      INT32 rc        = SDB_OK ;
      BOOLEAN bCreate = TRUE ;
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_HNDSNMSG );
      UINT64 sessionID = 0 ;
      _pmdAsyncSession *pSession = NULL ;

      // if opcode is disconnect or interrupt, we don't expect to create
      // new session
      if ( MSG_BS_DISCONNECT == header->opCode ||
           MSG_BS_INTERRUPTE == header->opCode ||
           MSG_BS_INTERRUPTE_SELF == header->opCode )
      {
         bCreate = FALSE ;
      }

      sessionID = _pSessionMgr->makeSessionID( handle, header ) ;

      // Find the associated session if exist
      // If the session doesn't exist, we'll check bCreate, if bCreate=TRUE it
      // will create one, otherwise will not
      rc = _pSessionMgr->getSession( sessionID ,
                                     PMD_SESSION_PASSIVE,
                                     handle, bCreate,
                                     header->opCode,
                                     NULL,
                                     &pSession ) ;
      // Determine whether a session is created or retreived
      if ( rc )
      {
         // If session is not retreived
         if ( !bCreate )
         {
            if ( MSG_BS_DISCONNECT == header->opCode )
            {
               _pSessionMgr->onNoneSessionDisconnect( sessionID ) ;
            }
            // It's okay if we don't expect one
            rc = SDB_OK ;
            goto done ;
         }
         // Otherwise log the message
         PD_LOG ( PDERROR, "Failed to create session[ID:%lld], rc: %d",
                  sessionID, rc ) ;

         rc = _pSessionMgr->onErrorHanding( rc, header, handle,
                                            sessionID, NULL ) ;
         if ( rc )
         {
            goto error ;
         }
         else
         {
            goto done ;
         }
      }

      // On recieve
      pSession->onRecieve ( handle, (_MsgHeader*)header ) ;

      // Check the received code
      if ( MSG_BS_DISCONNECT == header->opCode )
      {
         PD_LOG ( PDINFO, "Session[%s] recieved disconnect message",
                  pSession->sessionName() ) ;
         _pSessionMgr->onSessionDisconnect( pSession ) ;
         // Session will be released and we don't need to push message
         rc = _pSessionMgr->releaseSession( pSession, TRUE ) ;
         if ( rc )
         {
            PD_LOG ( PDWARNING, "Failed to release session, rc = %d", rc ) ;
            rc = SDB_OK ;
         }
         goto done ;
      }
      else if ( MSG_BS_INTERRUPTE == header->opCode )
      {
         PD_LOG ( PDINFO, "Session[%s] recieved interrupt message",
                  pSession->sessionName() ) ;
         pSession->eduCB()->interrupt() ;
         // For interrupt message, we have to continue in order to push the
         // message
      }
      else if ( MSG_BS_INTERRUPTE_SELF == header->opCode )
      {
         PD_LOG( PDEVENT, "Session[%s] recieved interrupt self message",
                 pSession->sessionName() ) ;
         pSession->eduCB()->interrupt() ;
         goto done ;
      }

      // push the mssage into session manager
      rc = _pSessionMgr->pushMessage( pSession, header, handle ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG ( PDERROR, "Failed to push message, rc = %d", rc ) ;

         rc = _pSessionMgr->onErrorHanding( rc, header, handle,
                                            sessionID, pSession ) ;
         if ( rc )
         {
            goto error ;
         }
         else
         {
            goto done ;
         }
      }

   done:
      PD_TRACE_EXITRC ( SDB__PMDMSGHND_HNDSNMSG, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_HNDMAINMSG, "_pmdAsyncMsgHandler::_handleMainMsg" )
   INT32 _pmdAsyncMsgHandler::_handleMainMsg( const NET_HANDLE &handle,
                                              const _MsgHeader *header,
                                              const CHAR *msg )
   {
      INT32 rc              = SDB_OK ;
      _MsgHeader *newHeader = NULL ;
      void *newMsg          = NULL ;
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_HNDMAINMSG );

      SDB_ASSERT( _pMgrEDUCB, "Main edu can't be NULL" ) ;

      // copy msg to a buffer and post the queue
      // the memory is allocated in _copyMsg and will be released by consumer
      newMsg = _copyMsg ( msg, header->messageLength ) ;
      if ( NULL == newMsg )
      {
         PD_LOG ( PDERROR, "Failed to allocate memory for new msg for %d bytes",
                  header->messageLength ) ;
         rc = SDB_OOM ;

         rc = _pSessionMgr->onErrorHanding( rc, header, handle, 0, NULL ) ;
         if ( rc )
         {
            goto error ;
         }
         else
         {
            goto done ;
         }
      }

      newHeader = ( MsgHeader * )newMsg ;
      _postMainMsg( handle, newHeader ) ;

   done:
      PD_TRACE_EXITRC ( SDB__PMDMSGHND_HNDMAINMSG, rc );
      return rc;
   error:
      goto done;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__PMDMSGHND_POSTMAINMSG, "_pmdAsyncMsgHandler::_postMainMsg" )
   void _pmdAsyncMsgHandler::_postMainMsg( const NET_HANDLE &handle,
                                           MsgHeader *pNewMsg )
   {
      PD_TRACE_ENTRY ( SDB__PMDMSGHND_POSTMAINMSG ) ;
      _pMgrEDUCB->postEvent( pmdEDUEvent ( PMD_EDU_EVENT_MSG,
                                           PMD_EDU_MEM_ALLOC,
                                           pNewMsg, (UINT64)handle ) ) ;
      PD_TRACE_EXIT ( SDB__PMDMSGHND_POSTMAINMSG ) ;
   }

}

