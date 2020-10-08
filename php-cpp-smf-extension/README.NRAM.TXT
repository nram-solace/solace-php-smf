Compiling
---
cd /Users/nram/Solace/Dev/PHP/solace-php-smf/php-cpp-smf-extension
make clean
make all
make install
make restart
make test 

# Manual Commands
sudo cp *.so /usr/local/lib/php/pecl/20190902
➜  php-cpp-smf-extension sudo apachectl stopphp -
➜  php-cpp-smf-extension sudo apachectl start