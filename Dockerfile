from riot/riotbuild

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    automake \
    python-setuptools \
    autoconf \
    pkg-config \
    libtool \
    libsodium-dev:i386 \
    libzmq3-dev:i386 \
    libczmq-dev:i386

RUN git clone --branch=v3.1.0 --depth=1 https://github.com/google/protobuf.git \
    && cd protobuf \ 
    && ./autogen.sh && ./configure CC="gcc -m32" CXX="g++ -m32"\
    && make -j 4 && make check && make install && ldconfig \
    && cd ./python \
    && python setup.py install \
    && cd ../../ \
    && rm -rf ./protobuf
    
RUN git clone --depth=1  https://github.com/protobuf-c/protobuf-c.git \
  && cd protobuf-c \
  && ./autogen.sh \
  && ./configure CC="gcc -m32" CXX="g++ -m32"\
  && make && make install \
  && cd ../ \
  && rm -rf ./protobuf-c
