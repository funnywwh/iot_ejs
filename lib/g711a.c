#include "duktape.h"
#include "log.h"
#include "utils.h"
static short aLawDecompressTable[] = 
        { -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736, -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784, -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392, -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944, -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136, -11008, -10496, -12032, -11520, -8960, -8448, -9984, -9472, -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568, -344, -328, -376,
                -360, -280, -264, -312, -296, -472, -456, -504, -488, -408, -392, -440, -424, -88, -72, -120, -104, -24, -8, -56, -40, -216, -200, -248, -232, -152, -136, -184, -168, -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184, -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, -688, -656, -752, -720, -560, -528, -624, -592, -944, -912, -1008, -976, -816, -784, -880, -848, 5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736, 7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784, 2752, 2624,
                3008, 2880, 2240, 2112, 2496, 2368, 3776, 3648, 4032, 3904, 3264, 3136, 3520, 3392, 22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944, 30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 11008, 10496, 12032, 11520, 8960, 8448, 9984, 9472, 15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568, 344, 328, 376, 360, 280, 264, 312, 296, 472, 456, 504, 488, 408, 392, 440, 424, 88, 72, 120, 104, 24, 8, 56, 40, 216, 200, 248, 232, 152, 136, 184, 168, 1376, 1312, 1504, 1440, 1120,
                1056, 1248, 1184, 1888, 1824, 2016, 1952, 1632, 1568, 1760, 1696, 688, 656, 752, 720, 560, 528, 624, 592, 944, 912, 1008, 976, 816, 784, 880, 848 };

static int cClip = 32635;
static unsigned char aLawCompressTable[] = 
        { 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };



static unsigned char linearToALawSample( short sample ){
    int sign;
    int exponent;
    int mantissa;
    int s;

    sign = ( ( ~sample ) >> 8 ) & 0x80;
    if ( !( sign == 0x80 ) )
    {
        sample = (short) -sample;
    }
    if ( sample > cClip )
    {
        sample = cClip;
    }
    if ( sample >= 256 )
    {
        exponent = (int) aLawCompressTable[( sample >> 8 ) & 0x7F];
        mantissa = ( sample >> ( exponent + 3 ) ) & 0x0F;
        s = ( exponent << 4 ) | mantissa;
    }
    else
    {
        s = sample >> 4;
    }
    s ^= ( sign ^ 0x55 );
    return (unsigned char) s;
}

/**
 * 编码  pcm  to  G711 a-law
 * @param b
 * @return
 */
int encode(unsigned char* in,int inSize,unsigned char* out,int outSize){
    int j = 0;
    int count = inSize / 2;
    int end = count;
    short sample = 0;

    for ( int i = 0; i < end; i++ )
    {
        sample = (short) ( ( ( in[j++] & 0xff ) | ( in[j++] ) << 8 ) );
        out[i] = linearToALawSample( sample );
    }
    return count;
}

/**
 * 解码
 * @param b
 * @return
 */
int decode(unsigned char* in,int inSize,unsigned char* out,int outSize){
    int j = 0;
    for ( int i = 0; i < inSize; i++ )
    {
        short s = aLawDecompressTable[in[i] & 0xff];
        out[j++] = (unsigned char) s;
        out[j++] = (unsigned char) ( s >> 8 );
    }
    return inSize*2;
}
static duk_ret_t g711a_constructor(duk_context *ctx){
    duk_push_this(ctx);
    return 1;
}
static duk_ret_t g711a_encode(duk_context *ctx){
    if(!duk_is_buffer(ctx,0)){
        duk_error(ctx,DUK_ERR_TYPE_ERROR,"arg0 is not buffer");
    }
    duk_size_t pcmSize = 0;
    unsigned char* pcm = duk_get_buffer_data(ctx,0,&pcmSize);
    if(pcmSize <= 0){
        duk_error(ctx,DUK_RET_RANGE_ERROR,"pcm buffer size is 0");
    }
    duk_push_fixed_buffer(ctx,pcmSize/2);
    duk_size_t g711aSize = 0;
    unsigned char* g711Buffer = duk_get_buffer_data(ctx,-1,&g711aSize);
    if(g711aSize <= 0){
        duk_error(ctx,DUK_ERR_ERROR,"Out of memory");
    }    
    encode(pcm,pcmSize,g711Buffer,g711aSize);
    return 1;
}
static duk_ret_t g711a_decode(duk_context *ctx){
    if(!duk_is_buffer_data(ctx,0)){
        debugf("arg0 type:%d",duk_get_type(ctx,0));
        duk_error(ctx,DUK_ERR_TYPE_ERROR,"arg0 is not buffer");
    }
    duk_size_t g711aSize = 0;
    unsigned char* g711aBuffer = duk_get_buffer_data(ctx,0,&g711aSize);
    if(g711aSize <= 0){
        duk_error(ctx,DUK_RET_RANGE_ERROR,"g711a buffer size is 0");
    }
    duk_push_fixed_buffer(ctx,g711aSize*2);
    duk_size_t pcmSize = 0;
    unsigned char* pcmBuffer = duk_get_buffer_data(ctx,-1,&pcmSize);
    if(g711aSize <= 0){
        duk_error(ctx,DUK_ERR_ERROR,"Out of memory");
    }        
    decode(g711aBuffer,g711aSize,pcmBuffer,pcmSize);
    return 1;
}
static duk_function_list_entry g711a_funcs[] = {
    {"iot.codec.G711A",g711a_constructor,1},
    {"encode",g711a_encode,1},
    {"decode",g711a_decode,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int g711a_init(duk_context* ctx){
    register_class(ctx,g711a_funcs);
    return 0;
}