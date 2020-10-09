<?php

    function php_msgReceiveCallback ($msg_dest,  $msg_body) {
        echo "=== php_msgReceiveCallback called ===\n" ;
        echo("Destination     : $msg_dest\n");
        echo ("Message :\n");
        echo ($msg_body);
        echo "\n===\n";
    }

    echo("calling solace-smf-wrapper\n");

    $solace_url = "localhost:55555";
    $vpn = "default" ;
    $user = "default" ;
    $pass = "default" ;
    $sub_topic = "test/php/>" ;
    $msg_callback_fn = "php_msgReceiveCallback" ;
    $verbose = 0;

    solcw_init($solace_url, $vpn, $user, $pass, $verbose);
    solcw_connect();
    solcw_subscribe_topic($sub_topic, $msg_callback_fn);

    sleep (120);

    solcw_unsubscribe_topic();
    solcw_cleanup();
    echo("done\n");
?>
