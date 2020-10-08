<?php
    echo("calling solace-smf-wrapper\n");

    echo(solcw_init("127.0.0.1:55555", "default", "default", "default", 0));
    echo(solcw_connect());
    for ($x = 0; $x < 2; $x++) {
        echo(solcw_publish_topic("test/php/$x", "Hello ($x) from PHP"));
    }
    echo(solcw_cleanup());
    echo("done\n");
?>
