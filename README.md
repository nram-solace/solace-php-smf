
# Solace PHP SMF Extension

## ABOUT

A PHP extension providing C bindings to [Solace](www.solace.com). 
Using the [PHP-CPP](http://www.php-cpp.com/) framework, this extension enables PHP scripts to call 
Solace APIs using native SMF (Solace message format) protocol with CCSMP libraries.

**Solace**: [Solace PubSub+ Platform](https://docs.solace.com/Solace-PubSub-Platform.htm) is a complete event streaming and management platform for real-time enterprises. PubSub+ helps enterprises design, deploy and manage event-driven architectures across hybrid cloud, multi-cloud and IoT environments.

PubSub+ event brokers power the **Event Mesh**, a modern messaging layer that can be deployed across every environment (Cloud, Software, Appliance) and component of the distributed enterprise to stream events across. PubSub+ APIs provide support for open protocols and APIs (MQTT, AMQP, JMS, REST) as well as proprietary messaging APIs. Solace APIs are available for several laugnages including Java, C, C++, JavaScript, .NET.

**SMF**: [Solace Message Format](https://docs.solace.com/PubSub-ConceptMaps/Component-Maps.htm#SMF) is the native transport protocol supported in Solace.

**CCSMP**: [CCSMP](https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/index.html) or SolClient is the Solace C bindings library supported by Solace.

**PHP-CPP**: [PHP-CPP](http://www.php-cpp.com/) is a C++ library for developing PHP extensions.

## DISCLAIMER

This is an experimental and prototype implementation. This is **not** a Solace product and not covered by Solace support.

## USAGE

Once you have the extension installed, you can use the following APIs to interact with Solace.

### Intializing Solace API & connecting to the broker

``` PHP
    solcc_init($solace_url, $vpn, $user, $pass, $verbose);
    solcc_connect();
```

The variable names are self explonatory. You will need to define these variables.

### Publish a message to a topic

``` PHP
    solcc_publish_topic("test/topic/1", "Hello world");
```

### Subscribing from a topic

This is slightly more involved than publishing to a topic. Once subcribed, messages will be delivered asynchronously to PHP.
So, first you have to define a callback function in PHP to receive messages and call subscribe function.

``` PHP
    function php_msgReceiveCallback ($msg_dest,  $msg_body) {
        echo "=== php_msgReceiveCallback called ===\n" ;
        echo("Destination     : $msg_dest\n");
        echo ("Message :\n");
        echo ($msg_body);
        echo "\n===\n";
    }

    solcc_subscribe_topic("test/topic/>", "php_msgReceiveCallback");
```

### Publishing to and Consuming from Queue

Its quite similar to topic publishing and subscribing.

### Sample scripts

[php folder](https://github.com/nram-dev/solace-php-smf/tree/master/php) has some sample scripts.
Pl take a look at solcc-topic-publisher.php for checks to make PHP callable from CLI as well as from browser.

### Calling from HTML

Call PHP scripts normally as you would from HTML. Some of these calls are blocking calls, so take that into consideration
in desigining responsive UI.
[HTML folder](https://github.com/nram-dev/solace-php-smf/tree/master/html) has HTML form samples.

## INSTALLATION

This has been tested to work on the following:

* CentOS 8
* MacOS Catalina
* SolOS 9.6
* PHP 7.3.X & 7.4.X

### 0. Prerequisites

* [PHP](https://www.php.net/) - Ofcourse
* [Apache](https://httpd.apache.org/) or any HTTP server
* [PHP-CPP](http://www.php-cpp.com/)

### 1. Makefile changes

In the **src/Makefile**, change the following to reflect your environment

``` sh
INI_DIR	= /usr/local/lib/php/pecl/20190902
CCSMPHOME:=/opt/Solace/solclient/latest/
```

### 2. Compile the code

``` sh
➜  src git:(master) make all
g++  -I/opt/Solace/solclient/latest/ -I/opt/Solace/solclient/latest//include -I. -D_LINUX_X86_64 -DPROVIDE_LOG_UTILITIES -g -Wall -c -O2 -std=c++11 -fpic -I ~/PHP/PHP-CPP -o solcc_extension.o solcc_extension.cpp
g++ -L/opt/Solace/solclient/latest//lib   -shared -o solace-smf-extension.so solcc_extension.o -lsolclient -lphpcpp
```

### 3. Install the extension

``` sh
➜  src git:(master) make install
sudo cp -fp solace-smf-extension.so /usr/local/Cellar/php/7.4.8/pecl/20190902
Password:
```

### 4. Edit **php.ini** file in your installation and add the following.

``` sh
➜  src git:(master) grep ^extension /opt/httpd/php/conf/conf/php.ini
extension=solace-smf-extension
```

### 5. Restart httpd

``` sh
➜  src git:(master) make restart
sudo apachectl stop
sleep 2
sudo apachectl start
```

### 6. Check if extension is loaded

``` sh
➜  src git:(master) php -f ../php/info.php| grep solace-smf
solace-smf-extension
```

You can also point your webbrowser to hit info.php and look for this.

## AUTHOR

Ramesh Natarajan