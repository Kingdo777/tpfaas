FROM faasm/cpython:0.0.12

RUN git config --global http.proxy http://211.69.198.232:8118
RUN git config --global https.proxy http://211.69.198.232:8118

WORKDIR /code
#RUN git clone https://github.com/Kingdo777/tpfaas.git
COPY . /code/tpfaas

WORKDIR /build
RUN cmake \
    -GNinja \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10 \
    -DCMAKE_C_COMPILER=/usr/bin/clang-10 \
    -DCMAKE_BUILD_TYPE=Release \
    /code/tpfaas

RUN cmake --build .

EXPOSE 8080

#COPY ../cmake-build-debug/bin/tpfaas .
#COPY ../config/nginx-server.conf /etc/nginx/sites-enabled/default
#COPY ../docker-entrypoint.sh .
#EXPOSE 8080
#ENTRYPOINT ["/action/docker-entrypoint.sh"]

