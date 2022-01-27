#!/bin/sh

lastno=`ls -1 ss[0-9]*.xwd 2>/dev/null | tail -1 | sed 's/ss\([0-9]*\).*/\1/'`
let "nextno= ${lastno} + 1"
if [ -n "$1" ] ; then
xwd -id "$1" -out "ss${nextno:-1}.xwd"
else
xwd -root -out "ss${nextno:-1}.xwd"
fi
