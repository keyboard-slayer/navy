#include <abstract/arch.h>
#include <base/debug.h>
#include <base/macro.h>

noreturn void bootstrap(void)
{
    println$("Hello, World !");
    loop {}
    block_return();
}