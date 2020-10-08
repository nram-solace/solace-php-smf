/*****************************************************************************************
 * php-cpp-smf-extension
 *    This file implements SMF (Solace Message Format) wrapper using Solace CCSMP APIs (Solace C library)
 *    This is an expermimental and prototype implementation
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

/* Session */
solClient_opaqueSession_pt session_p;

solClient_rxMsgCallback_returnCode_t messageReceiveCallback ( solClient_opaqueSession_pt opaqueSession_p, solClient_opaqueMsg_pt msg_p, void *user_p )
{
    return SOLCLIENT_CALLBACK_OK;
}

void eventCallback ( solClient_opaqueSession_pt opaqueSession_p,
                solClient_session_eventCallbackInfo_pt eventInfo_p, void *user_p )
{
    if ( ( eventInfo_p->sessionEvent ) == SOLCLIENT_SESSION_EVENT_ACKNOWLEDGEMENT )
        printf ( "Acknowledgement received!\n" );
}

int RC = 0 ; // If RC is not 0, don't proceed
int Verbose = 0;


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
void solcw_publish( Php::Parameters &params) {

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
    Php::out << "Sending message to destination :" << dest << std::endl;
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
        extension.add<solcw_publish>("solcw_publish");
        extension.add<solcw_cleanup>("solcw_cleanup");
        return extension;
    }
}
