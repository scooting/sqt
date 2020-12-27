/*
Copyright (c) 2018-2021 Imran Hameed
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#if defined(_MSC_VER)
namespace msvcrt_support {

struct mtx_t {
    alignas(void *) char opaque[40];
};

bool
mtx_init(mtx_t &);

void
mtx_uninit(mtx_t &);

void
mtx_lock(mtx_t &);

void
mtx_unlock(mtx_t &);

struct condvar_t {
    alignas(void *) char opaque[sizeof(void *)];
};

void
condvar_init(condvar_t &);

void
condvar_uninit(condvar_t &);

bool
condvar_wait_timeout(condvar_t &, mtx_t &, unsigned __int32 timeout);

void
condvar_notify_all(condvar_t &);

void
internal_get_environ(char ***value);

}
#endif
