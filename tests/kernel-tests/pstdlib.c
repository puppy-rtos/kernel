#include <puppy.h>
#include <stdlib.h>
#define TEST_SIZE 256

void test_mem_api(void)
{
    char *buf;
    buf = malloc(TEST_SIZE);
    if(!buf)
    {
        P_TC_FAIL();
    }
    for(int i = 0; i < TEST_SIZE; i++)
    {
        buf[i] = i;
        if (buf[i] != i) P_TC_FAIL();
    }
    free(buf);
    P_TC_PASS();
}
P_TC_FUNC(test_mem_api, posix.mem.tc);