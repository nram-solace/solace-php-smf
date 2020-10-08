/*****************************************************************************************
 * php-cpp-smf-extension
 *    This file implements SMF (Solace Message Format) wrapper using Solace CCSMP APIs (Solace C library)
 *    This is an expermimental and prototype implementation
 * 
 * Requires
 *  PHP-CPP (https://www.php-cpp.com/)
 * 
 *  Usage
 *     See php/test-xxx.php and html/test-xxx.html files
 * 
 *  Author
 *    Ramesh Natarajan, Solace
 *    Oct 08, 2020
 *****************************************************************************************/
#include "os.h"
#include "solclient/solClient.h"
#include "solclient/solClientMsg.h"
// php-cpp stuff
#include <phpcpp.h>
#include <iostream>

/***************************************************************************************
 * Globals 
 * FIXME: Get rid of globals
 * See https://www.php-cpp.com/documentation/extension-callbacks
 ****************************************************************************************/

// save session globally to access later
solClient_opaqueSession_pt session_p;

// save dest for unsubscribe
std::string g_dest ; // TODO: this should be an array

/* message callback function name from php*/
std::string g_php_msg_recv_cb_fn ;

/* Message Count */
static int g_msgcount = 0;

int RC = 0 ; // If RC is not 0, don't proceed
int Verbose = 0;

/*****************************************************************************
 * messageReceiveCallback
 *
 * The message callback is invoked for each Direct message received by
 * the Session. In this sample, the message is printed to the screen.
 *****************************************************************************/
solClient_rxMsgCallback_returnCode_t
messageReceiveCallback ( solClient_opaqueSession_pt opaqueSession_p, solClient_opaqueMsg_pt msg_p, void *user_p )
{
    if (Verbose) {
        Php::out << "Received message: " << g_msgcount << std::endl ;
        solClient_msg_dump ( msg_p, NULL, 0 ); printf ( "\n" );
    }

    solClient_destination_t msg_dest_p ;
    void * msg_body_p = NULL;
    solClient_uint32_t msg_body_sz;
    //const char *msg_id_p = NULL;
    //solClient_int64_t msg_rcv_ts_p ;

    /*
     * extract message details from the message ptr
     */
    solClient_msg_getDestination(msg_p, &msg_dest_p, sizeof(msg_dest_p)) ;
    Php::out << "(" << g_msgcount << ") Received a message on destination  : " << msg_dest_p.dest  << std::endl ;
  
    solClient_msg_getBinaryAttachmentPtr (msg_p, &msg_body_p, &msg_body_sz) ;
    char * msg_body_cp = (char *)msg_body_p ;
    msg_body_cp[msg_body_sz] = 0 ;
    if (Verbose) Php::out << "msg_body_p : " << msg_body_cp << std::endl ;

    /*
     * TODO: other things to extract 
     * FIXME: This causes core dump 
     * 
    solClient_msg_getRcvTimestamp(msg_p, &msg_rcv_ts_p);
    Php::out << "msg_rcv_ts_p   : " << msg_rcv_ts_p  << std::endl ;

    solClient_msg_getApplicationMessageId(msg_p, &msg_id_p);
    Php::out << "msg_id_p   : " << msg_id_p  << std::endl ;
    */

    /*
     * Call the PHP callback function with message details
     * FYI, This (call back PHP) is not recommended according to PHP-CPP site
     */
    Php::Value(g_php_msg_recv_cb_fn.c_str())( (char *)(msg_dest_p.dest),  
                                               msg_body_cp);

    g_msgcount++;

    return SOLCLIENT_CALLBACK_OK;
}

/*****************************************************************************
 * eventCallback
 *
 * The event callback function is mandatory for session creation.
 *****************************************************************************/
void eventCallback ( solClient_opaqueSession_pt opaqueSession_p,
                solClient_session_eventCallbackInfo_pt eventInfo_p, void *user_p )
{
    if ( ( eventInfo_p->sessionEvent ) == SOLCLIENT_SESSION_EVENT_ACKNOWLEDGEMENT )
        Php::out << "Acknowledgement received!" << std::endl ;
}




/*************************************************************************
 * Initialize the API, Context, session
 *************************************************************************/
void solcw_init( Php::Parameters &params) {
    if (Verbose) Php::out << "### solcw_init called (1)"<< std::endl;
    
    /*
    for (auto &p : params)  {
      Php::out << "param :" << p << std::endl;
    }
    */

    std::string host =  (std::string)(params[0]) ;
    std::string vpn  =  (std::string)(params[1]) ;
    std::string user =  (std::string)(params[2]) ;
    std::string pass =  (std::string)(params[3]) ;
    Verbose = params[4] ;
    if (Verbose) {
        Php::out << "host :" << host << std::endl;
        Php::out << "vpn  :" << vpn << std::endl;
        Php::out << "user :" << user << std::endl;
    }

    /* Context */
    solClient_opaqueContext_pt context_p;
    solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

    /* Session */
    solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;

    /* Session Properties */
    const char     *sessionProps[20];
    int             propIndex = 0;

    /* solClient needs to be initialized before any other API calls. */
    Php::out << "Initalizing Solace session ..." << std::endl;
    solClient_initialize ( SOLCLIENT_LOG_DEFAULT_FILTER, NULL );

    /* 
     * Create a Context, and specify that the Context thread should be created 
     * automatically instead of having the application create its own
     * Context thread.
     */
    if (Verbose) Php::out << "Creating Solace context..." << std::endl;
    solClient_context_create ( SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                                           &context_p, &contextFuncInfo, sizeof ( contextFuncInfo ) );

    /*
     * Message receive callback function and the Session event function
     * are both mandatory. In this sample, default functions are used.
     */
    if (Verbose) Php::out << "Configuring callbacks ..." << std::endl;
    sessionFuncInfo.rxMsgInfo.callback_p = messageReceiveCallback;
    sessionFuncInfo.rxMsgInfo.user_p = NULL;
    sessionFuncInfo.eventInfo.callback_p = eventCallback;
    sessionFuncInfo.eventInfo.user_p = NULL;

    /* Configure the Session properties. */
    if (Verbose) Php::out << "Configuring session properties ..." << std::endl;
    propIndex = 0;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_HOST;
    sessionProps[propIndex++] = host.c_str();

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_VPN_NAME;
    sessionProps[propIndex++] = vpn.c_str();

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_USERNAME;
    sessionProps[propIndex++] = user.c_str();

    if (Verbose) {
        Php::out << "SOLCLIENT_SESSION_PROP_HOST     : " <<  sessionProps[1] << std::endl;
        Php::out << "SOLCLIENT_SESSION_PROP_VPN_NAME : " <<  sessionProps[3] << std::endl;
        Php::out << "SOLCLIENT_SESSION_PROP_USERNAME : " <<  sessionProps[5] << std::endl;
    }
    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_PASSWORD;
    sessionProps[propIndex++] = pass.c_str();

    sessionProps[propIndex] = NULL;

    /* Create the Session. */
    Php::out << "Creating Solace session to host " << host << " vpn " << vpn << " user " << user << "..." << std::endl;
    RC = solClient_session_create ( ( char ** ) sessionProps,
                               context_p,
                               &session_p, 
                               &sessionFuncInfo, 
                               sizeof ( sessionFuncInfo ) );
    if (Verbose) Php::out << "Create session rc: " << RC << std::endl;

}

/*************************************************************************
 * Connect to session
 *************************************************************************/
void solcw_connect () {
    if (Verbose) Php::out << "### solcw_connect called "<< std::endl;

    if (RC != 0) {
        Php::out << "*** Something isn't right. Can't proceed" << std::endl;
        return ;
    }
    /* Connect the Session. */
    if (Verbose) Php::out << "Connecting to Solace session ..." << std::endl;
    RC = solClient_session_connect ( session_p );
    if (Verbose) Php::out << "Connect session rc: " << RC << std::endl;
    if (RC == 0) Php::out << "Connected to Solace session ..." << std::endl;
}

/*************************************************************************
 * Publish
 *************************************************************************/
void solcw_publish_topic ( Php::Parameters &params) {

    if (Verbose) Php::out << "### solcw_publish called" << std::endl;
    if (RC != 0) {
        Php::out << "*** Something isn't right. Can't proceed" << std::endl;
        return ;
    }
    /* Message */
    solClient_opaqueMsg_pt msg_p = NULL;
    solClient_destination_t destination;


    std::string dest = (std::string)(params[0]) ;
    std::string data = (std::string)(params[1]) ;

    /* Allocate a message. */
    Php::out << "Sending message to topic :" << dest << std::endl;
    solClient_msg_alloc ( &msg_p );

    /* Set the delivery mode for the message. */
    solClient_msg_setDeliveryMode ( msg_p, SOLCLIENT_DELIVERY_MODE_DIRECT );

    /* Set the destination. */
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = dest.c_str();
    solClient_msg_setDestination ( msg_p, &destination, sizeof ( destination ) );

    /* Add some content to the message. */
    solClient_msg_setBinaryAttachment ( msg_p, data.c_str(), ( solClient_uint32_t ) data.length() );

    /* Send the message. */
    if (Verbose) {
        Php::out << "Sending message :" << data << std::endl;
        solClient_msg_dump ( msg_p, NULL, 0 );
    }
    solClient_session_sendMsg ( session_p, msg_p );
    if (Verbose) Php::out << "Message sent!" << std::endl;

    /* Free the message. */
    if (Verbose) Php::out << "Free message ..." << std::endl;
    solClient_msg_free ( &msg_p );

    /* Sleep to allow the message to be acknowledged. */
    SLEEP ( 2 );
}


/*************************************************************************
 * Subscribe
 *************************************************************************/
void solcw_subscribe_topic ( Php::Parameters &params) {

    if (Verbose) Php::out << "### solcw_subribe_topic called" << std::endl;
    if (RC != 0) {
        Php::out << "*** Something isn't right. Can't proceed" << std::endl;
        return ;
    }

    std::string dest = (std::string)(params[0]) ;
    // save destination & callback fn name globally
    g_dest = dest;
    g_php_msg_recv_cb_fn = (std::string)(params[1]) ;

    Php::out << "Subscribing to topic : " << dest << " callback: " << g_php_msg_recv_cb_fn << std::endl;

    RC = solClient_session_topicSubscribeExt ( session_p,
                                          SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM,
                                          dest.c_str() );
}

/*************************************************************************
 * Unsubscribe
 *************************************************************************/
void solcw_unsubscribe_topic () {

    if (Verbose) Php::out << "### solcw_unsubscribe_topic called" << std::endl;
    if (RC != 0) {
        Php::out << "*** Something isn't right. Can't proceed" << std::endl;
        return ;
    }
    //std::string dest = (std::string)(params[0]) ;
    if (Verbose) Php::out << "Unubscribing to topic :" << g_dest << std::endl;

    RC = solClient_session_topicUnsubscribeExt ( session_p,
                                          SOLCLIENT_SUBSCRIBE_FLAGS_WAITFORCONFIRM,
                                          g_dest.c_str() );
}

/*************************************************************************
 * CLEANUP & exit
 *************************************************************************/
void solcw_cleanup() {

    if (Verbose) Php::out << "### solcw_cleannup called" << std::endl;
    if (RC != 0) {
        Php::out << "*** Something isn't right. Can't proceed" << std::endl;
        return ;
    }
    /* Cleanup solClient. */
    RC = solClient_cleanup ();
}


/*************************************************************************
 * Register all functions
 *************************************************************************/
extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("solace-smf-wrapper", "1.0");
        extension.add<solcw_init>("solcw_init");
        extension.add<solcw_connect>("solcw_connect");
        extension.add<solcw_publish_topic>("solcw_publish_topic");
        extension.add<solcw_subscribe_topic>("solcw_subscribe_topic");
        extension.add<solcw_unsubscribe_topic>("solcw_unsubscribe_topic");
        extension.add<solcw_cleanup>("solcw_cleanup");
        return extension;
    }
}
