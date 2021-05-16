#!/bin/bash
cgi-fcgi -start -connect 127.0.0.1:8088 tpfaas
# 手动启动nginx
nginx -g "daemon off;"
