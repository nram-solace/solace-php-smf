<?php

    $solace_url = "localhost:55555";
    $vpn = "default" ;
    $user = "default" ;
    $pass = "default" ;
    $topic_prefix = "test/php" ;

    $verbose = 1;
    $nmsgs = 10 ;

    echo("calling solace-smf-wrapper\n");

    solcc_init($solace_url, $vpn, $user, $pass, $verbose);
    echo(solcc_connect());
    for ($x = 0; $x < 10; $x++) {
        $topic = "$topic_prefix/$x" ;
        $data = "Hello ($x) from PHP to topic" ;
        echo(solcc_publish_topic($topic, $data));
        # FIXME: Allow time for ack
        sleep(1);
    }
    echo(solcc_cleanup());
    echo("done\n");
?>
