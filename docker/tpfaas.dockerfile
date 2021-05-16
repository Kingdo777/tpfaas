FROM kingdo/fcgi
FROM nginx

WORKDIR /action
COPY ../cmake-build-debug/bin/tpfaas .
COPY ../config/nginx-server.conf /etc/nginx/sites-enabled/default
COPY ../docker-entrypoint.sh .
EXPOSE 8080
ENTRYPOINT ["/action/docker-entrypoint.sh"]

