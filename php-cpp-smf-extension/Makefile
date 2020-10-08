#
#	Makefile template
#
#	This is an example Makefile that can be used by anyone who is building
#	his or her own PHP extensions using the PHP-CPP library. 
#
#	In the top part of this file we have included variables that can be
#	altered to fit your configuration, near the bottom the instructions and
#	dependencies for the compiler are defined. The deeper you get into this
#	file, the less likely it is that you will have to change anything in it.
#

#
#	Name of your extension
#
#	This is the name of your extension. Based on this extension name, the
#	name of the library file (name.so) and the name of the config file (name.ini)
#	are automatically generated
#

NAME				=	solace-smf-wrapper


#
#	Php.ini directories
#
#	In the past, PHP used a single php.ini configuration file. Today, most
#	PHP installations use a conf.d directory that holds a set of config files,
#	one for each extension. Use this variable to specify this directory.
#

INI_DIR				=	/usr/local/lib/php/pecl/20190902

# Dir with PHP scripts
PHP_DIR = ../php
PHP_TEST_SCRIPT = sol-test1.php


#
#	The extension dirs
#
#	This is normally a directory like /usr/lib/php5/20121221 (based on the 
#	PHP version that you use. We make use of the command line 'php-config' 
#	instruction to find out what the extension directory is, you can override
#	this with a different fixed directory
#

EXTENSION_DIR		=	$(shell php-config --extension-dir)


#
#	The name of the extension and the name of the .ini file
#
#	These two variables are based on the name of the extension. We simply add
#	a certain extension to them (.so or .ini)
#

EXTENSION 			=	${NAME}.so
INI 				=	${NAME}.ini


#
#	Compiler
#
#	By default, the GNU C++ compiler is used. If you want to use a different
#	compiler, you can change that here. You can change this for both the 
#	compiler (the program that turns the c++ files into object files) and for
#	the linker (the program that links all object files into the single .so
#	library file. By default, g++ (the GNU C++ compiler) is used for both.
#

COMPILER			=	g++
LINKER				=	g++


#
#	Compiler and linker flags
#
#	This variable holds the flags that are passed to the compiler. By default, 
# 	we include the -O2 flag. This flag tells the compiler to optimize the code, 
#	but it makes debugging more difficult. So if you're debugging your application, 
#	you probably want to remove this -O2 flag. At the same time, you can then 
#	add the -g flag to instruct the compiler to include debug information in
#	the library (but this will make the final libphpcpp.so file much bigger, so
#	you want to leave that flag out on production servers).
#
#	If your extension depends on other libraries (and it does at least depend on
#	one: the PHP-CPP library), you should update the LINKER_DEPENDENCIES variable
#	with a list of all flags that should be passed to the linker.
#

# Solace stuff
CCSMPHOME:=/opt/Solace/solclient/latest/
INCDIRS:=-I${CCSMPHOME} \
	 -I$(CCSMPHOME)/include \
	 -I.
ARCHFLAGS:=-D_LINUX_X86_64
SIXTY_FOUR_COMPAT:=
LINUXLIBDIR=$(CCSMPHOME)/obj_Linux26-x86_64_debug/lib
LIBDIRS:=-L$(CCSMPHOME)/lib #-L$(LINUXLIBDIR)
LLSYS:= $(SIXTY_FOUR_COMPAT)
SOLCW_COMPILEFLAG:= $(COMPILEFLAG) $(INCDIRS) $(ARCHFLAGS) -DPROVIDE_LOG_UTILITIES -g
SOLCW_LINKFLAGS:= $(LIBDIRS) $(LLSYS)
SOLCW_LIBS=-lsolclient

COMPILER_FLAGS		=	${SOLCW_COMPILEFLAG} -Wall -c -O2 -std=c++11 -fpic -I ~/PHP/PHP-CPP -o
LINKER_FLAGS		=	${SOLCW_LINKFLAGS} -shared
LINKER_DEPENDENCIES	=	${SOLCW_LIBS} -lphpcpp


#
#	Command to remove files, copy files and create directories.
#
#	I've never encountered a *nix environment in which these commands do not work. 
#	So you can probably leave this as it is
#

RM					=	rm -f
CP					=	cp -f
MKDIR				=	mkdir -p


#
#	All source files are simply all *.cpp files found in the current directory
#
#	A builtin Makefile macro is used to scan the current directory and find 
#	all source files. The object files are all compiled versions of the source
#	file, with the .cpp extension being replaced by .o.
#

SOURCES				=	$(wildcard *.cpp)
OBJECTS				=	$(SOURCES:%.cpp=%.o)


#
#	From here the build instructions start
#

all:					${OBJECTS} ${EXTENSION}

${EXTENSION}:			${OBJECTS}
						${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} ${LINKER_DEPENDENCIES}

${OBJECTS}:
						${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

install:		
						sudo ${CP} ${EXTENSION} ${EXTENSION_DIR}
						#${CP} ${INI} ${INI_DIR}

# Restart Apache
restart:		
						sudo apachectl stop
						sleep 2
						sudo apachectl start

# Run test script
test:
						php -f ${PHP_DIR}/${PHP_TEST_SCRIPT}	
			
clean:
						${RM} ${EXTENSION} ${OBJECTS}

