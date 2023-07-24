#/bin/bash

# Don't forget to pull uplink submodule

pushd ./uplink-c
go build -o ../libuplinkc.a -buildmode=c-archive
cp *.h ../
popd

make