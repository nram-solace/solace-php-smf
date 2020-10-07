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

void solcw_publish( Php::Parameters &params) {

    Php::out << "### solcw_publish called" << std::endl;

    /* Message */
    solClient_opaqueMsg_pt msg_p = NULL;
    solClient_destination_t destination;

    // ugly hack to extract info from std::vector. 
    // there got to be a more elegant way
    char *values[2];
    short c = 0;
    for (auto &p : params)  {
        //Php::out << "param :" << p << std::endl;
	values[c++] = strdup(p);
    }
    const char *dest_p = values[0];
    const char *text_p = values[1];


    /*************************************************************************
     * Publish
     *************************************************************************/

    /* Allocate a message. */
    Php::out << "Prepare message to destination :" << dest_p << std::endl;
    solClient_msg_alloc ( &msg_p );

    /* Set the delivery mode for the message. */
    solClient_msg_setDeliveryMode ( msg_p, SOLCLIENT_DELIVERY_MODE_DIRECT );

    /* Set the destination. */
    destination.destType = SOLCLIENT_TOPIC_DESTINATION;
    destination.dest = dest_p;
    solClient_msg_setDestination ( msg_p, &destination, sizeof ( destination ) );

    /* Add some content to the message. */
    solClient_msg_setBinaryAttachment ( msg_p, text_p, ( solClient_uint32_t ) strlen ( (char *)text_p ) );

    /* Send the message. */
    Php::out << "Sending message :" << text_p << std::endl;
    //solClient_msg_dump ( msg_p, NULL, 0 );
    solClient_session_sendMsg ( session_p, msg_p );
    Php::out << "Message sent!" << std::endl;

    /* Free the message. */
    Php::out << "Free message ..." << std::endl;
    solClient_msg_free ( &msg_p );

    /* Sleep to allow the message to be acknowledged. */
    SLEEP ( 2 );
}

void solcw_init( Php::Parameters &params) {

    Php::out << "### solcw_init called "<< std::endl;
    
    // ugly hack to extract info from std::vector. 
    // there got to be a more elegant way
    char *values[5];
    short c = 0;
    for (auto &p : params)  {
        //Php::out << "param :" << p << std::endl;
	values[c++] = strdup(p);
    }

    /* Context */
    solClient_opaqueContext_pt context_p;
    solClient_context_createFuncInfo_t contextFuncInfo = SOLCLIENT_CONTEXT_CREATEFUNC_INITIALIZER;

    /* Session */
    solClient_session_createFuncInfo_t sessionFuncInfo = SOLCLIENT_SESSION_CREATEFUNC_INITIALIZER;

    /* Session Properties */
    const char     *sessionProps[20];
    int             propIndex = 0;


    /*************************************************************************
     * Initialize the API (and setup logging level)
     *************************************************************************/

    /* solClient needs to be initialized before any other API calls. */
    Php::out << "Initalizing progrem ..." << std::endl;
    solClient_initialize ( SOLCLIENT_LOG_DEFAULT_FILTER, NULL );

    /*************************************************************************
     * Create a Context
     *************************************************************************/

    /* 
     * Create a Context, and specify that the Context thread should be created 
     * automatically instead of having the application create its own
     * Context thread.
     */
    Php::out << "Create Solace context..." << std::endl;
    solClient_context_create ( SOLCLIENT_CONTEXT_PROPS_DEFAULT_WITH_CREATE_THREAD,
                                           &context_p, &contextFuncInfo, sizeof ( contextFuncInfo ) );

    /*************************************************************************
     * Create and connect a Session
     *************************************************************************/

    /*
     * Message receive callback function and the Session event function
     * are both mandatory. In this sample, default functions are used.
     */
    Php::out << "Configure callbacks ..." << std::endl;
    sessionFuncInfo.rxMsgInfo.callback_p = messageReceiveCallback;
    sessionFuncInfo.rxMsgInfo.user_p = NULL;
    sessionFuncInfo.eventInfo.callback_p = eventCallback;
    sessionFuncInfo.eventInfo.user_p = NULL;

    /* Configure the Session properties. */
    Php::out << "Configure session properties ..." << std::endl;
    propIndex = 0;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_HOST;
    sessionProps[propIndex++] = values[0];
    Php::out << "SOLCLIENT_SESSION_PROP_HOST : " <<  sessionProps[1] << std::endl;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_VPN_NAME;
    sessionProps[propIndex++] = values[1];
    Php::out << "SOLCLIENT_SESSION_PROP_VPN_NAME : " <<  sessionProps[3] << std::endl;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_USERNAME;
    sessionProps[propIndex++] = values[2];
    Php::out << "SOLCLIENT_SESSION_PROP_USERNAME : " <<  sessionProps[5] << std::endl;

    sessionProps[propIndex++] = SOLCLIENT_SESSION_PROP_PASSWORD;
    sessionProps[propIndex++] = values[3];

    sessionProps[propIndex] = NULL;

    /* Create the Session. */
    Php::out << "Create Solace session ..." << std::endl;
    solClient_session_create ( ( char ** ) sessionProps,
                               context_p,
                               &session_p, &sessionFuncInfo, sizeof ( sessionFuncInfo ) );

}

void solcw_connect () {
    /* Connect the Session. */
    Php::out << "### solcw_connect called "<< std::endl;
    Php::out << "Connect to Solace session ..." << std::endl;
    solClient_session_connect ( session_p );
    Php::out << "Connected to Solace session ..." << std::endl;
}

void solcw_cleanup() {
    /*************************************************************************
     * CLEANUP
     *************************************************************************/
    Php::out << "### solcw_cleannup called" << std::endl;

    /* Cleanup solClient. */
    solClient_cleanup (  );
}

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
