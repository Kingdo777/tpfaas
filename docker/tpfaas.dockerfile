FROM faasm/cpython:0.0.12 as python
FROM faasm/cpp-root:0.5.16
COPY --from=python /usr/local/faasm/runtime_root /usr/local/faasm/runtime_root

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

RUN cmake --build . --target simple_runner
RUN cmake --build . --target func_runner
RUN cmake --build . --target codegen_func
RUN cmake --build . --target codegen_shared_obj
RUN cmake --build . --target pool_runner

EXPOSE 8080

CMD "/build/bin/pool_runner"

#COPY ../cmake-build-debug/bin/tpfaas .
#COPY ../config/nginx-server.conf /etc/nginx/sites-enabled/default
#COPY ../docker-entrypoint.sh .
#EXPOSE 8080
#ENTRYPOINT ["/action/docker-entrypoint.sh"]

