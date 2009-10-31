#!/bin/bash
ver=0.2
dfold="release-$ver"

if [ ! -d "$dfold" ]; then
    echo "No such folder '$dfold'"
    exit 1
fi

cd "$dfold"
rev=`svn info | grep Revision | awk '{print $2}'`
cd ..
findir="mojito-$ver-r$rev"
cp -rv "$dfold" "$findir" || exit 2
cd "$findir"
/bin/rm todo || exit 2
/bin/rm -rf `find . -iname '\.svn'` || exit 2
cd ..
tar cjf "$findir.tar.bz2" "$findir"
exit 0
