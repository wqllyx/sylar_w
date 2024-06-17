
#pragma once

#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>

namespace sylar_w
{

    /**
     * @brief 返回当前线程的ID
     */
    pid_t GetThreadId();

    /**
     * @brief 返回当前协程的ID
     */
    uint32_t GetFiberId();

}
