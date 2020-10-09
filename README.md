# Solace PHP SMF Extension
## ABOUT
A PHP extension providing C bindings to [Solace](www.solace.com). 
Using the [PHP-CPP](http://www.php-cpp.com/) framework, this extension enables PHP scripts to call 
Solace APIs using native SMF (Solace message format) protocol with CCSMP libraries.

### Solace
[Solace PubSub+](https://docs.solace.com/) is a an advanced and versatile event broker supporting a number of protocols (such as SMF, JMS, MQTT, AMQP, REST) and language bindings (C, .NET, Java,...).
As of Oct 2020, PHP is not natively supported.

### SMF
[Solace Message Format](https://docs.solace.com/PubSub-ConceptMaps/Component-Maps.htm#SMF) is the native transport supported by Solace.

### CCSMP
[CCSMP](https://docs.solace.com/API-Developer-Online-Ref-Documentation/c/index.html) or SolClient is the Solace C bindings library supported by Solace.

### PHP-CPP
[PHP-CPP](http://www.php-cpp.com/) is a C++ library for developing PHP extensions. 

## DISCLAIMER
This is an experimental and prototype implementation. This is **not** a Solace product and not covered by Solace support.

## USAGE
Once you have the extension installed, you can use the following APIs to interact with Solace.

### Intializing Solace API & connecting to the broker
```
    solcc_init($solace_url, $vpn, $user, $pass, $verbose);
    solcc_connect();
```
The variable names self explonatory. You will need to define these variables.

### Publish a message to a topic
```
    solcc_publish_topic("test/topic/1", "Hello world");
```

### Subscribing from a topic
This is slightly more involved than publish. Once subcribed, messages will be delivered asynchronously. 
So first you have to define a callback function in PHP to receive messages and call subscribe function
```
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
Its quite similar to topic publishing and subcribing.

### Calling from HTML
Call PHP scripts as you would call from HTML. Some of these calls are blocking calls, so take that into consideration
in desigining responsive UI.

### Sample scripts
[php folder](https://github.com/nram-dev/solace-php-smf/tree/master/php) has some sample scripts.
[HTML folder](https://github.com/nram-dev/solace-php-smf/tree/master/html) has some sample HTML form samples.

## INSTALLATION

### 0. Prerequisites
* [PHP](https://www.php.net/) - Ofcourse
* [Apache](https://httpd.apache.org/) or any HTTP server
* [PHP-CPP](http://www.php-cpp.com/)

### 1. Makefile changes
In the **src/Makefile**, change the following to reflect your environment
```
INI_DIR				=	/usr/local/lib/php/pecl/20190902
CCSMPHOME:=/opt/Solace/solclient/latest/
```

### 2. Compile the code

```
➜  src git:(master) make all
g++  -I/opt/Solace/solclient/latest/ -I/opt/Solace/solclient/latest//include -I. -D_LINUX_X86_64 -DPROVIDE_LOG_UTILITIES -g -Wall -c -O2 -std=c++11 -fpic -I ~/PHP/PHP-CPP -o solcc_extension.o solcc_extension.cpp
g++ -L/opt/Solace/solclient/latest//lib   -shared -o solace-smf-extension.so solcc_extension.o -lsolclient -lphpcpp
```

### 3. Install the extension
```
➜  src git:(master) make install
sudo cp -fp solace-smf-extension.so /usr/local/Cellar/php/7.4.8/pecl/20190902
Password:
```

### 4. Edit **php.ini** file in your installation and add the following.
```
➜  src git:(master) grep ^extension /opt/httpd/php/conf/conf/php.ini
extension=solace-smf-extension
```

### 5. Restart httpd
```
➜  src git:(master) make restart
sudo apachectl stop
sleep 2
sudo apachectl start
```

### 6. Check if extension is loaded
```
➜  src git:(master) php -f ../php/info.php| grep solace-smf
solace-smf-extension
```
You can also point your webbrowser to hit info.php and look for this.

