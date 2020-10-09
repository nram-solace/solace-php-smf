<?php

    function php_msgReceiveCallback ($msg_dest,  $msg_body) {
        echo "=== php_msgReceiveCallback called ===\n" ;
        echo("Destination     : $msg_dest\n");
        echo ("Message :\n");
        echo ($msg_body);
        echo "\n===\n";
    }


    $solace_url = "localhost:55555";
    $vpn = "default" ;
    $user = "default" ;
    $pass = "default" ;
    $qname = "TestQ" ;
    $msg_callback_fn = "php_msgReceiveCallback" ;
    $verbose = 1;

    solcw_init($solace_url, $vpn, $user, $pass, $verbose);
    solcw_connect();
    solcw_subscribe_queue($qname, $msg_callback_fn);

    sleep (120);

    solcw_cleanup();
    echo("done\n");
?>
