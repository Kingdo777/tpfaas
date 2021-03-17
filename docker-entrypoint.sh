#!/bin/bash
fcgi/fcgi -start -connect 127.0.0.1:8088 tpfaas
nginx -g "daemon off;"
