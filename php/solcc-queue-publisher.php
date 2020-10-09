<?php

    $solace_url = "localhost:55555";
    $vpn = "default" ;
    $user = "default" ;
    $pass = "default" ;
    $qname = "TestQ" ;
    $NON_PERSISTENT = 1 ;
    $PERSISTENT = 2;
    //$msg_callback_fn = "php_msgReceiveCallback" ;
    $verbose = 1;
    $nmsgs = 10 ;

    echo("calling solace-smf-wrapper\n");

    solcc_init($solace_url, $vpn, $user, $pass, $verbose);
    solcc_connect();
    for ($x = 0; $x < $nmsgs; $x++) {
        $data = "Hello ($x) from PHP to Q" ;
        solcc_publish_queue ($qname, $data, $PERSISTENT);
        # FIXME: Allow time for ack
        sleep(1) ;
    }
    solcc_cleanup();
    echo("done\n");
?>
