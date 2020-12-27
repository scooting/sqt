; Copyright (c) 2018-2021 Imran Hameed
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.

; This stack probing procedure avoids double-probing pages by checking the stack
; limit. The stack limit can be loaded directly from gs:[10h] on Windows and
; Wine on Linux, FreeBSD, etc. This doesn't work on Wine on OS X, because OS X
; uses gs for its own thread-specific data. Wine on OS X does set the linear
; address of the TIB in gs:[30h], which is a TSD slot that is not used by OS X's
; pthreads implementation, so that is used here as a workaround. This adds an
; extra data dependent load to the hot path of this procedure, but functions
; that call alloca or have huge activation records are rare.
.code
_msvcrt_support_chkstk proc frame
    push rcx ; save the old value of rcx
    .allocstack 8
    push rdx ; save the old value of rdx
    .allocstack 8
    .endprolog

    mov  rdx, qword ptr gs:[30h] ; load the linear address of the TIB
    mov  rdx, qword ptr [rdx + 10h] ; load the current thread's TIB stack limit
    lea  rcx, [rsp + 24] ; load the address of the former bottom of the stack--we've pushed rcx and rdx; the caller has pushed a return address
    sub  rcx, rax ; subtract the total delta from the former bottom of the stack
    jc   crash ; if rcx - rax wraps around, crash
    cmp  rcx, rdx ; compare the requested stack limit with the TIB stack limit
    jae  done ; if the requested stack limit is above than the TIB stack limit, return
one_page:
    test [rdx], rdx ; read from the probe address
    sub  rdx, 1000h ; subtract one page from the probe address
    cmp  rcx, rdx ; compare the requested stack limit with the probe address
    jb   one_page ; repeat if the requested stack limit is below the probe address
done:
    pop  rdx ; restore rdx
    pop  rcx ; restore rcx
    ret
crash:
    mov byte ptr [0h], 0
_msvcrt_support_chkstk endp
end
