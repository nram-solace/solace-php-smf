<?php
    echo("calling solace-smf-wrapper\n");

    echo(solcw_init("localhost:55555", "default", "default", "default"));
    echo(solcw_connect());
    echo(solcw_publish("test/php/1", "Hello from PHP"));
    echo(solcw_publish("test/php/2", "Hello again from php"));
    echo(solcw_cleanup());

    echo("done\n");
?>
