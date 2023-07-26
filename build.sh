#/bin/bash

# Don't forget to pull uplink submodule

pushd ./uplink-c
go build -o ../libuplinkc.a -buildmode=c-archive
cp *.h ../
popd

make

BASE_IMAGE_NAME="storj-test"

docker rmi gsc-${BASE_IMAGE_NAME} -f
docker rmi gsc-${BASE_IMAGE_NAME}-unsigned -f
docker rmi ${BASE_IMAGE_NAME} -f

docker build -t ${BASE_IMAGE_NAME} --rm .
rm -f enclave-key.pem
openssl genrsa -3 -out enclave-key.pem 3072

rm -rf ./gsc
git clone https://github.com/gramineproject/gsc && cd gsc

./gsc build ${BASE_IMAGE_NAME} ../test.manifest -c ../gsc-config.yaml
./gsc sign-image ${BASE_IMAGE_NAME} ../enclave-key.pem -c ../gsc-config.yaml
