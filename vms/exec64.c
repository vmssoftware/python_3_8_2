#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

extern __char_ptr32 _strdup32(__const_char_ptr64);
extern __void_ptr32 _malloc32(size_t);

int execve64(const __char_ptr64 file_spec, __char_ptr64 argv[], __char_ptr64 envp[])
{
    int i, n, m;
    __char_ptr_ptr32 argv32 = NULL;
    __char_ptr_ptr32 envp32 = NULL;
    int rv;
    n = 0;

    while (argv[n]) {
        n++;
    }

    argv32 = (__char_ptr_ptr32) _malloc32((n + 1) * sizeof(char *));
    assert(argv32);

    for (i = 0; i < n; i++) {
        argv32[i] = _strdup32(argv[i]);
        assert(argv32[i]);
    }

    argv32[n] = NULL;

    m = 0;

    while (envp[m]) {
        m++;
    }

    envp32 = (__char_ptr_ptr32) _malloc32((m + 1) * sizeof(char *));
    assert(envp32);

    for (i = 0; i < m; i++) {
        envp32[i] = _strdup32(envp[i]);
        assert(envp32[i]);
    }

    envp32[m] = NULL;

    rv = execve(file_spec, argv32, envp32);

    /* If all is well we will not get here, but just in case the execv() failed let's have a bit of a tidy-up... */
    for (i = 0; i < n; i++) {
        free(argv32[i]);
    }
    free(argv32);

    for (i = 0; i < m; i++) {
        free(envp32[i]);
    }
    free(envp32);

    /* ... and return our status */
    return (rv);
}

int execv64(const __char_ptr64 file_spec, __char_ptr64 argv[])
{
    int i, n;
    __char_ptr_ptr32 argv32 = NULL;
    int rv;
    n = 0;

    while (argv[n]) {
        n++;
    }

    argv32 = (__char_ptr_ptr32) _malloc32((n + 1) * sizeof(char *));
    assert(argv32);

    for (i = 0; i < n; i++) {
        argv32[i] = _strdup32(argv[i])
        assert(argv32[i]);
    }

    argv32[n] = NULL;

    rv = execv(file_spec, argv32);

    /* If all is well we will not get here, but just in case the execv() failed let's have a bit of a tidy-up... */
    for (i = 0; i < n; i++) {
        free(argv32[i]);
    }
    free(argv32);

    /* ... and return our status */
    return (rv);
}


int execvp64(const __char_ptr64 file_spec, __char_ptr64 argv[])
{
    int i, n;
    __char_ptr_ptr32 argv32 = NULL;
    int rv;
    n = 0;

    while (argv[n]) {
        n++;
    }

    argv32 = (__char_ptr_ptr32) _malloc32((n + 1) * sizeof(char *));
    assert(argv32);

    for (i = 0; i < n; i++) {
        argv32[i] = _strdup32(argv[i])
        assert(argv32[i]);
    }

    argv32[n] = NULL;

    rv = execvp(file_spec, argv32);

    /* If all is well we will not get here, but just in case the execv() failed let's have a bit of a tidy-up... */
    for (i = 0; i < n; i++) {
        free(argv32[i]);
    }
    free(argv32);

    /* ... and return our status */
    return (rv);
}
