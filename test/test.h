//
// Created by kingdo on 2020/10/27.
//

#ifndef TPFAAS_TEST_H
#define TPFAAS_TEST_H

#include <resource/resource.h>
#include <function/function.h>
#include <string.h>
#include <task/task.h>
#include <sync/sync.h>
#include <unistd.h>
#include <tool/time_k.h>
#include "tool/list_head.h"

extern list_head task_list_head;
extern list_head func_list_head;

void taskF(FCGX_Request *request) {
    size_t contentLen;
    char *requestMethod, *content;
    requestMethod = FCGX_GetParam("REQUEST_METHOD", request->envp);
    FCGX_FPrintF(request->out, "Content-type: text/json\r\n\r\n");
    for (char **envp = request->envp; *envp != NULL; envp++) {
        FCGX_FPrintF(request->out, "%s \n", *envp);
    }
    if (!strcmp("GET", requestMethod)) {
        FCGX_FPrintF(request->out, "<h1>%s</h1>", requestMethod);
    } else {
        contentLen = strtoul(FCGX_GetParam("CONTENT_LENGTH", request->envp), NULL, 10);
        content = (char *) malloc(contentLen);
        FCGX_GetStr(content, contentLen, request->in);
        FCGX_FPrintF(request->out, "<h1>%s</h1>" "<p>%s</p>", requestMethod, content);
        free(content);
    }
    FCGX_Finish_r(request);
    free(request);
}

static void function_test() {
    resource res = DEFAULT_RESOURCE;
    func_register(taskF, "taskA", res, 1);
    func_register(taskF, "taskB", res, 10);
    F *f;
    list_for_each_entry_prev(f, &func_list_head, func_list_list) {
        const char *func_name = f->name;
        printf("%s RUN:\n", func_name);
        FUN(func_name, "hello world\n")
    }
}

#endif //TPFAAS_TEST_H
