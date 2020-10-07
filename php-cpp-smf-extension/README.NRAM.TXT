Compiling
---
cd /Users/nram/Solace/Dev/PHP/solace-php-smf/php-cpp-smf-extension
make
sudo cp *.so /usr/local/lib/php/pecl/20190902

Restarting apache
---
➜  php-cpp-smf-extension sudo apachectl stop
➜  php-cpp-smf-extension sudo apachectl start