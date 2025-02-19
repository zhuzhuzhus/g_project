#ifndef __EPC_CARD_H__
#define __EPC_CARD_H__
#include <stdint.h>

typedef struct epc_card
{
    char *name;
    char *user_data;
    uint64_t timestamp;
    struct epc_card *next;
}epc_card_t;

void card_init();
#endif