#include <stdint.h>
#include <am335x/pru_cfg.h>

char* test = "bullshit";

typedef struct {
int horseshit;
int bullshit;
} goatshit;

//goatshit test1;

goatshit test1 = {.horseshit = 0};

//int main() {
volatile  pruCfg CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
//}
