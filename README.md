#INIT
autoscan

#Development
aclocal
autoconf
autoheader
automake --add-missing --copy
#Na innym systemie
Before running ./configure try running 
autoreconf -f -i. 
The autoreconf program automatically runs autoheader, aclocal, automake, autopoint and libtoolize as required.
