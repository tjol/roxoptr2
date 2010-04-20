#!/bin/sh

echo +++ Running aclocal ... &&
aclocal &&
echo +++ Running autoconf ... &&
autoconf && 
echo +++ Running automake ... &&
automake --foreign Makefile source/Makefile &&
echo You may now run ./configure ||
( echo ERROR.; false )

