<?php

    $solace_url = "localhost:55555";
    $vpn = "default" ;
    $user = "default" ;
    $pass = "default" ;
    $qname = "TestQ" ;
    $NON_PERSISTNET = 1 ;
    $PERSISTENT = 2;
    //$msg_callback_fn = "php_msgReceiveCallback" ;
    $verbose = 1;
    $nmsgs = 10 ;

    echo("calling solace-smf-wrapper\n");

    solcw_init($solace_url, $vpn, $user, $pass, $verbose);
    solcw_connect();
    for ($x = 0; $x < $nmsgs; $x++) {
        solcw_publish_queue ($qname, "Hello ($x) from PHP to Q", $PERSISTENT);
    }
    solcw_cleanup();
    echo("done\n");
?>
