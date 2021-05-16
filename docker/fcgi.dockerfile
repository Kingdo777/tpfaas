FROM ubuntu:focal

RUN apt-get update
RUN apt-get install -y software-properties-common

RUN apt install -y \
   gcc \
   make \
   m4 \
   autoconf \
   automake \
   libtool  \
   git

WORKDIR /setup

RUN git clone https://github.com/FastCGI-Archives/fcgi2.git
WORKDIR /setup/fcgi2
RUN ./autogen.sh    &&\
    ./configure     &&\
    make            &&\
    make install
ENV LD_LIBRARY_PATH='/usr/local/lib'