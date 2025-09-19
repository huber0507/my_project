#ifndef __IMX6UL_H
#define __IMX6UL_H

#include "cc.h"
//#include "core_ca7.h"       /* 原始位置我是放置是在这里，会报错“未定义标识符__GIC_PRIO_BITS””*/
                                                            // 总结：先备齐 “基础材料”，再写 “使用材料的逻辑”
                                                            //如果取消注释，在最后还是加上#include "core_ca7.h"   还是会报原来一样的错误
#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "core_ca7.h"     
#endif

