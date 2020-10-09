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

    solcc_init($solace_url, $vpn, $user, $pass, $verbose);
    solcc_connect();
    solcc_subscribe_topic($sub_topic, $msg_callback_fn);

    sleep (120);

    solcc_unsubscribe_topic();
    solcc_cleanup();
    echo("done\n");
?>
