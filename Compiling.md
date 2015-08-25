﻿#summary Compiling
#labels Phase-Deploy,Featured

# The fast and easy way #
The faster and easier way to have a working environment for Mojito is to make a specific folder, copy the needed files and run it. Expressed as a bash-script, for version 0.2-[r73](https://code.google.com/p/mojito-webserver/source/detail?r=73):
```
# create a folder
mkdir mojito_folder
cd mojito_folder
# download and compile
wget "http://mojito-webserver.googlecode.com/files/mojito-0.2-r73.tar.bz2" -O mojito.tar.bz2 
tar xjf mojito.tar.bz2
cd mojito-0.2-r73
make all
# copy needed files up
cp mojito ..
cp config.ini ..
cd ..
# remove the sources
rm -rf mojito-0.2-r73
rm mojito.tar.bz2
# ajust the config.ini file to fit your needs. The important part is to specify the http_root parameter!
vim config.ini
# start Mojito
./mojito
```

# Pre-requisites #
Any linux environment should do. The main builds right know are done on a Gentoo box (kernel-2.6.30, gcc-4.3.2, glibc-2.9), but on all the linux boxes on which I tried to compile Mojito everything went fine.

You need the ability to build with zlib too.


---

# Compiling #
Do a "`make clean all`" on the root folder.


---

# Installing #
There is no install facility right now (aka no "`make install`"), so you need to follow the following two steps:

## Installing Mojito manually ##
You have to manually move the file named "mojito" to the desired folder. Typically you will want to copy the "config.ini" template too, and modify the parameters according to your environment.

## Installing the Mojito's modules manually ##
If (_and only if_) you build with dynamic modules, you will want to copy the built modules too. A "`find <projectpath> -name '*.so.1'`" will tell you which files you have to move/copy. Change the "_module\_basepath_" option in the "config.ini" file to point to the desired folder.


---

# Makefiles (compile-time options) #

## Main Makefile ##
The Makefiles are a bit tricky. The most important switches are located in the Makefile.flags on the project root folder.
  * the **PSTATUS** variable accepts two values: "debug" and "release". The first one will activate the debugging informations (-g) and deactivates the optimizations (-O0) on gcc for all the .c files, while the second one will activate optimizations (-O3) and set the NDEBUG macro.
  * the **MODULE** variable accept three values: "lstatic", "lstripped" and "lshared". _lstatic_ will link all the modules statically to the core. _lstripped_ will statically link only the really needed modules (some filters, no cache, no statistics, no cgi, etc.). _lshared_ will not link anything, giving the possibility to load everything dynamically.

## Modules Makefiles ##
The modules Makefiles always build **both** the static **and** the shared version of them.

## IMPORTANT NOTE ##
Right now the Makefiles are set to build a **debug** and **dynamic** version by default. If you want to help us debugging Mojito leave them as-they-are, but for a more stable program you probably will want to change them to build a static-release version.


---

# Running #
I suggest you to **run mojito in a chrooted environment**. To start it, just run `./mojito` from it's folder.