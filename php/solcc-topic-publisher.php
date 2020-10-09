<?php

    $verbose = 0;

    function is_cli()
    {
        if( defined('STDIN') ) { return true; }
        if( empty($_SERVER['REMOTE_ADDR']) and !isset($_SERVER['HTTP_USER_AGENT']) and count($_SERVER['argv']) > 0) 
        { return true; } 
        return false;
    }

    if (is_cli()) {
        echo("Called from CLI\n");
        $solace_url = "localhost:55555";
        $vpn = "default" ;
        $user = "default" ;
        $pass = "default" ;
        $topic_prefix = "test/php" ;
        $nmsgs = 10 ; ;
    }
    else {
        echo("Called from HTTP\n");
        $solace_url = $_POST["url"] ;
        $vpn = $_POST["vpn"] ;
        $user = $_POST["user"] ;
        $pass = $_POST["pass"] ;
        $topic_prefix = $_POST["topic"] ;
        #$nmsgs = $_POST["nmsgs"] ; 
        $nmsgs = 2; # don't let browser hang too long 
    }

    if ($verbose) {
        echo ("solace-topic-publisher.php called:\n");
        echo ("url  : $vpn\n");
        echo ("vpn  : $vpn\n");
        echo ("user : $vpn\n");
        echo ("topic: $topic_prefix\n");
    }

    solcc_init($solace_url, $vpn, $user, $pass, $verbose);
    echo(solcc_connect());
    for ($x = 0; $x < $nmsgs; $x++) {
        $topic = "$topic_prefix/$x" ;
        $data = "Hello ($x) from PHP to topic" ;
        echo(solcc_publish_topic($topic, $data));
        # FIXME: Allow time for ack
        sleep(1);
    }
    echo(solcc_cleanup());
    echo("done\n");
?>
