#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef void* (*malloc_func_t)(size_t size);
typedef void (*free_func_t)(void* ptr);

malloc_func_t sys_malloc = NULL;
free_func_t sys_free = NULL;

void* malloc(size_t size) {
	void* ptr = sys_malloc(size);
    fprintf(stderr, "malloc: ptr= %p, lenth=%ld\n", ptr, size);
    return ptr;
}

void free(void *ptr) {
	fprintf(stderr, "free: ptr =%p\n", ptr);
	sys_free(ptr);
}


int main() {
    
	sys_malloc = dlsym(RTLD_NEXT, "malloc");
	assert(dlerror() == NULL);
	sys_free = dlsym(RTLD_NEXT, "free");
	assert(dlerror() == NULL);

	char* ptrs[5];

	for (int i = 0; i < 5; i ++) {
		ptrs[i] = malloc(100 + i);
		memset(ptrs[i], 0, 100 + i);
	}
	for (int i = 0; i < 5; i ++) {
		free(ptrs[i]);
	}
	return 0;
}