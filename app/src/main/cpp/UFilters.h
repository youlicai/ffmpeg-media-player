//
// Created by apple on 2020/9/29.
//

#ifndef FACEU_UFILTERS_H
#define FACEU_UFILTERS_H
#include <stdio.h>
#include <stdlib.h>
extern "C"{
#include "libavfilter/avfilter.h"
};
class UFilters {
public:
    const char *filters_descr = "drawtext=fontfile=../simhei.ttf:fontcolor=black:fontsize=50:text='zhuweigang'";
    //图表FilterGraph
     AVFilterGraph *m_filter_graph = NULL;
//输入filter的context
     AVFilterContext *buffersrc_ctx = NULL;
//输出filter的context
     AVFilterContext *buffersink_ctx = NULL;

public:
    void init_filters();
};


#endif //FACEU_UFILTERS_H
