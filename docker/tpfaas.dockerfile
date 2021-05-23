FROM faasm/cpython:0.0.12

WORKDIR /code
RUN git clone https://github.com/Kingdo777/tpfaas.git

WORKDIR /code/tpfaas
RUN mkdir build &&\
    cd build &&\
    cmkake ..&&\
    cmake --build .
EXPOSE 8080

#COPY ../cmake-build-debug/bin/tpfaas .
#COPY ../config/nginx-server.conf /etc/nginx/sites-enabled/default
#COPY ../docker-entrypoint.sh .
#EXPOSE 8080
#ENTRYPOINT ["/action/docker-entrypoint.sh"]

