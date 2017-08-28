pushd; cd sphinxbase;   ./autogen.sh; ; ./configure prefix=`pwd`/build; make; make install; popd
pushd; cd pocketsphinx; ./autogen.sh; ; ./configure prefix=`pwd`/build; make; make install; popd
