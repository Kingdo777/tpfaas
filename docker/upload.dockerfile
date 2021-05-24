FROM kingdo/tpfaas

WORKDIR /build

RUN cmake --build . --target upload

EXPOSE 8002

CMD "/build/bin/upload"