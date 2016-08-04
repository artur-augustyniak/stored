#include "mtab_check_trigger.h"

void trigger_check(void (*f)(void)){
    f();
}