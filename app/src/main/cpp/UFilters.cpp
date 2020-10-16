//
// Created by apple on 2020/9/29.
//

#include "UFilters.h"

#define WIDTH 352
#define HEIGHT 288
#define VIEO_PIX 0       //AV_PIX_FMT_YUV420P
#define TIMEBASE_NUM 1
#define TIMEBASE_DEN 25
#define SAR_X        1
#define SAR_Y        1

void UFilters::init_filters() {
    int ret = 0;
    char video_info_args[512] = {0};                                               //视频的基本信息，包括宽高，timebase等填入的字符串数组
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");                         //输入filter
    AVFilter *buffersink = avfilter_get_by_name("buffersink");                     //输出filter
    AVFilterInOut *outputs = avfilter_inout_alloc();                               //输入filter的pin，名称out
    AVFilterInOut *inputs  = avfilter_inout_alloc();                               //输出filter的pin,名称in

    //初始化图表filter graph,filter分为grarph，输入pin和输出pin
    m_filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !m_filter_graph)
    {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    //宽，高，pix格式，timebase(codec层，如果是裸流可以填写帧率1：25)，SAR(通过DAR和宽高算出的，不知道可以填写1：1)
//    _snprintf(video_info_args, sizeof(video_info_args),
//              "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
//              WIDTH, HEIGHT, VIEO_PIX,
//              TIMEBASE_NUM, TIMEBASE_DEN,
//              SAR_X,SAR_Y);

    //创建输入filter，并通过输入filter获取到该filter的context信息
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       video_info_args, NULL, m_filter_graph);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    //创建输出filter，并通过输出filter获取到该filter的context信息
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, m_filter_graph);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The m_filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    //解析连接两个pin的命令
    if ((ret = avfilter_graph_parse_ptr(m_filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0)
    {
        goto end;
    }

    //检查并连接filter
    if ((ret = avfilter_graph_config(m_filter_graph, NULL)) < 0)
    {
        goto end;
    }

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);


}