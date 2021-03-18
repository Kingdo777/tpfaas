FROM ubuntu:focal
RUN apt-get update && apt-get install -y \
    gcc \
    nginx \
    && mkdir /action
WORKDIR /action
COPY fcgi/.libs /action/fcgi/.libs
COPY fcgi/cgi-fcgi /action/fcgi/fcgi
COPY fcgi/lib/* /lib/
COPY build/tpfaas /action/tpfaas
COPY config/nginx-server.conf /etc/nginx/sites-enabled/default
COPY docker-entrypoint.sh .
EXPOSE 8080
ENTRYPOINT ["/action/docker-entrypoint.sh"]

