#include "common.h"
#include "wd_crt_input.h"
#include "wd_sample_ui.h"
#ifndef WD_CRT_SAMPLE
float4 sample_panel_texel(ui_vert_out I) {
    float4 c=tex2D(s_base,I.tc0);
#ifdef WD_GREYSCALE
    c.rgb=dot(c.rgb,float3(0.299,0.587,0.114));
#endif
    return c*I.c;
}
#define WD_CRT_SAMPLE(I) sample_panel_texel(I)
#endif
float4 main(ui_vert_out I):COLOR0 {return WD_CRT_SAMPLE(I);}
