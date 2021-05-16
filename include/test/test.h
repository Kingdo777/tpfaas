//
// Created by kingdo on 2020/10/27.
//

#ifndef TPFAAS_TEST_H
#define TPFAAS_TEST_H


#include <fcgiapp.h>
#include <cstring>
#include <cstdlib>
#include <rapidjson/document.h>

using namespace rapidjson;

void handle(char *req, char *res) {
    printf("%s\n", req);
    Document document;
    document.Parse(req);
    if (!document.HasParseError() &&
        document.HasMember("value")) {
        const Value &value = document["value"];
        if (value.IsObject() && value.HasMember("name")) {
            const Value &name = document["name"];
            sprintf(res, "%s", name.GetString());
            return;
        }
    }
    sprintf(res, "req \n`%s`\n format wrong\n", req);
}

void taskF(FCGX_Request *request) {
    size_t contentLen;
    char *content, *uri;
    uri = FCGX_GetParam("SCRIPT_NAME", request->envp);
    if (!strcmp("/", uri)) {
        FCGX_FPrintF(request->out, "Content-type: application/json\r\n"
                                   "Content-length: %d\r\n"
                                   "\r\n"
                                   "Hello,World\n", strlen("Hello,World\n"));
    } else if (!strcmp("/init", uri)) {
        FCGX_FPrintF(request->out, "Content-type: application/json\r\n"
                                   "Content-length: %d\r\n"
                                   "\r\n"
                                   "{\"ok\":true}\n", strlen("{\"ok\":true}\n"));
    } else if (!strcmp("/run", uri)) {
        contentLen = strtoul(FCGX_GetParam("CONTENT_LENGTH", request->envp), NULL, 10);
        content = (char *) malloc(contentLen);
        FCGX_GetStr(content, contentLen, request->in);
        handle(content, content);
        FCGX_FPrintF(request->out, "Content-type: application/json\r\n"
                                   "Content-length: %d\r\n"
                                   "\r\n"
                                   "%s", strlen(content), content);
        free(content);
    }
    FCGX_Finish_r(request);
    free(request);
}

void taskT(FCGX_Request *request) {
    printf("Kingdo %ld\n", (unsigned long) request);
}

#endif //TPFAAS_TEST_H
