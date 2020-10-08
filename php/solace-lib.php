<?php
    echo("calling solace-smf-wrapper\n");

    echo(solcw_init($_POST["broker_url"], $_POST["vpnname"], $_POST["client_username"], $_POST["client_password"]));
    echo(solcw_connect());
    echo(solcw_publish($_POST["topic_name"], "Hello from PHP"));
    echo(solcw_cleanup());

    echo("done\n");
?>
