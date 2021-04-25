//
// Created by kingdo on 2020/10/27.
//

#ifndef TPFAAS_TEST_H
#define TPFAAS_TEST_H


#include <fcgiapp.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"

void handle(char *req, char *res) {
    char data[1000];
    cJSON *root = cJSON_Parse(req);
    sprintf(data, "Hello,%s", cJSON_GetObjectItem(cJSON_GetObjectItem(root, "value"), "name")->valuestring);
    cJSON *content = cJSON_Parse("{}");
    cJSON *msg = cJSON_Parse("{}");
    cJSON_AddStringToObject(content, "C", data);
    cJSON_AddItemToObject(msg, "msg", content);
    strcpy(res, cJSON_Print(msg));
    cJSON_Delete(root);
    cJSON_Delete(msg);
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
    printf("Kingdo %d\n", (int) request);
}

#endif //TPFAAS_TEST_H
