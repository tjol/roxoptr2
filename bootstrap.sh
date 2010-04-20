#!/bin/sh

gendatamakefile () {
    if which hg >/dev/null && test -d .hg
    then
        files=`hg manifest | grep "^$1/"`
    else
        echo WARNING: hg is not installed, or this is not in a Mercurial repo!
        echo "         falling back to find."
        files=`find leveldata -type f -not \( -name '*Makefile*' -o -name '*~' -o -name '.*.sw?' \)`
    fi
    tempd=`mktemp -d`
    echo -n > $1/Makefile.am
    for f in $files
    do
        dirn=`dirname $f`
        mf_dirn=`echo $dirn | sed 's/[^a-zA-Z]//g'`
        relf=`echo $f | sed "s/^$1.//"`
        echo >$tempd/${mf_dirn}dir "${mf_dirn}dir = \$(pkgdatadir)/${dirn}"
        echo -n >>$tempd/dist_${mf_dirn}_DATA "$relf "
    done

    for f in $tempd/*dir
    do cat $f >>$1/Makefile.am
    done

    for f in $tempd/*DATA
    do echo >>$1/Makefile.am `basename $f` = `cat $f`
    done

    rm -rf $tempd
}

echo +++ Generating leveldata/Makefile.am ... &&
gendatamakefile leveldata &&
echo +++ Generating sprites/Makefile.am ... &&
gendatamakefile sprites &&
echo +++ Running aclocal ... &&
aclocal &&
echo +++ Running autoconf ... &&
autoconf && 
echo +++ Running automake --add-missing ... &&
automake --add-missing --foreign &&
echo +++ Running automake ... &&
automake --foreign Makefile source/Makefile leveldata/Makefile sprites/Makefile &&
echo You may now run ./configure ||
( echo ERROR.; false )

