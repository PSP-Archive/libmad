// libmadライブラリを用いたMP3再生補助関数群
//                               create by ままん 05/07/20
//    URL:http://jview.nomaki.jp/
//  email:maman_jv@excite.co.jp
//  
#include "main.h"

// for repeat play
#define LOOP

// sampling buffer size
// 1152の倍数なのはMADのデコードサイズと揃えるため
#define SAMPLE_SIZE (1152*3)

static int decode(unsigned char const *, unsigned long);

static int fsize(int fd)
{
  int ret;
  
  sceIoLseek(fd, (long long int)0, SCE_SEEK_SET);
  ret = sceIoLseek(fd, (long long int)0, SCE_SEEK_END);
  sceIoLseek(fd, (long long int)0, SCE_SEEK_SET);

  return ret;
}

// mp3play
//  fname  : 再生ファイル名
//
// オンメモリにMP3データを読み込み再生を行なう
// .zip.mp3 や ID3v2タグ形式のMP3は再生不可能
// 再生周波数は44.1kHzのみ
int mp3play(char *fname)
{
  int i;
  int ret;
  
  // init audio channel
  if(minf.handle == 0) {
    minf.handle = sceAudioChReserve(-1, SAMPLE_SIZE, 0);
  }
  
  // open mp3 file
  minf.fd = sceIoOpen(fname, O_RDONLY, 0777);
  if(minf.fd < 0) return -1;

  // check file size
  minf.fsize = fsize(minf.fd);
  
  // create MP3 buffer
  minf.buf_mp3 = (void *)calloc(minf.fsize);
  sceIoRead(minf.fd, minf.buf_mp3, minf.fsize);

  // create 16bit-signed short 2channel stereo WAVE buffer
  minf.buf_wav[0] = (signed short *)calloc(SAMPLE_SIZE*sizeof(signed short)*2);
  minf.buf_wav[1] = (signed short *)calloc(SAMPLE_SIZE*sizeof(signed short)*2);
  
  // start decode if all init right done
  if (minf.handle > 0 && minf.fd > 0 &&
     minf.fsize > 0 && minf.buf_mp3 != NULL &&
     minf.buf_wav[0] != NULL && minf.buf_wav[1] != NULL) {
    // start decode and play mp3
    ret = decode(minf.buf_mp3, minf.fsize);
  } else {
    ret = -1;
  }

  // free buffer
  for(i=0;i<2;i++) {
    if(minf.buf_wav[i] != NULL) {
      free(minf.buf_wav[i]);
      minf.buf_wav[i] = NULL;
    }
  }
  if(minf.buf_mp3 != NULL) {
    free(minf.buf_mp3);
    minf.buf_mp3 = NULL;
  }

  // free handle
  if(minf.handle > 0) {
    sceAudioChRelease(minf.handle);
    minf.handle = 0;
  }
  if(minf.fd > 0) {
    sceIoClose(minf.fd);
    minf.fd = 0;
  }
  minf.fsize = 0;
  
  return ret;
}

struct buffer {
  unsigned char const *start;
  unsigned long length;
};

// Callback function input buffer
static enum mad_flow input(void *data,
                           struct mad_stream *stream)
{
  struct buffer *buffer = data;
  int rsize;
  int idx;
  int ret;

  // 再生終了フラグ
  if(!buffer->length) return MAD_FLOW_STOP;

  mad_stream_buffer(stream, minf.buf_mp3, buffer->length);

  // 読み込み済みサイズを更新
  // ループ再生の場合はlengthの値は変更しない
#ifndef LOOP
  buffer->length = 0;
#endif
  
  return MAD_FLOW_CONTINUE;
}

// convert MAD format to signed short WAVE format
static inline signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

// Callback function after decode
static enum mad_flow output(void *data,
                            struct mad_header const *header,
                            struct mad_pcm *pcm)
{
  unsigned int nchannels, nsamples;
  mad_fixed_t const *left_ch, *right_ch;
  static int cnt = 0;
  static int idx = 0;
  int vol = 0x8000;

  nchannels = pcm->channels;
  nsamples  = pcm->length;
  left_ch   = pcm->samples[0];
  right_ch  = pcm->samples[1];
  
  while (nsamples--) {
    signed short left,right;
    left = scale(*left_ch++);
    right = scale(*right_ch++);

    // output sample(s) in 16-bit signed little-endian PCM
    minf.buf_wav[idx][cnt++] = (signed short)left;
    if (nchannels == 2) {
      minf.buf_wav[idx][cnt++] = (signed short)right;
    } else {
      // for mono
      minf.buf_wav[idx][cnt++] = (signed short)left;
    }
      
    // 再生バッファが一杯になったら再生
    if(cnt == SAMPLE_SIZE*2) {
      sceAudioOutputPannedBlocking(minf.handle,vol,vol,minf.buf_wav[idx]);
      // swap buffer
      idx = idx?0:1;
      cnt = 0;
    }
  }

  return MAD_FLOW_CONTINUE;
}

static enum mad_flow error(void *data,
                           struct mad_stream *stream,
                           struct mad_frame *frame)
{
  struct buffer *buffer = data;

  return MAD_FLOW_CONTINUE;
}

static int decode(unsigned char const *start, unsigned long length)
{
  struct buffer buffer;
  struct mad_decoder decoder;
  int result;

  buffer.start  = start;
  buffer.length = length;

  mad_decoder_init(&decoder, &buffer,
                   input, 0 /* header */, 0 /* filter */, output,
                   error, 0 /* message */);

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  mad_decoder_finish(&decoder);

  return result;
}

