/*
 * @FilePath: \cppServer-Win\day02\util.cpp
 * @Description:  handle error
 * @Author: rthete
 * @Date: 2023-03-04 21:25:43
 * @LastEditTime: 2023-03-04 21:43:46
 */
#include "util.h"
#include <stdio.h>
#include <stdlib.h> // exit()

void errif(bool condition, const char *errmsg) {
    if(condition) {
        perror(errmsg);     // 打印errno的实际意义与errmsg
        exit(EXIT_FAILURE); // 异常退出，等价于exit(1)
    }
}