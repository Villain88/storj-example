FROM ubuntu:22.04

RUN mkdir -p /test
COPY ./test /test/test
COPY ./64K /test/64K
WORKDIR /test

ENTRYPOINT ["/test/test"]
