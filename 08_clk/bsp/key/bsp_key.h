#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "imx6ul.h"

enum keyvalue{
    KEY_NONE = 0,
    KEY0_VALUE,
};

/*函数声明*/

void key_init(void);
int key_getvalue(void);


#endif // !_BSP_KEY_H
