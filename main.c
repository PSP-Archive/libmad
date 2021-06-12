// main.c
// メインとかスレッドやCallback周りの関数
// 

#include "main.h"

volatile int bSleep=0;

sceMemoryMgr MemTable[TABLE_SIZE];
MP3_INFO minf;

// ホームボタン終了時にコールバック
int exit_callback(void)
{
  scePowerSetClockFrequency(222,222,111);

  sceKernelExitGame();

  return 0;
}

// スリープ時や不定期にコールバック
void power_callback(int unknown, int pwrflags)
{
  // スリープを含むパワースイッチOFF時だけ各種保存処理
  if(pwrflags & POWER_CB_POWER){
    bSleep=1;
    
    scePowerSetClockFrequency(222,222,111);
  }
  // コールバック関数の再登録
  // （一度呼ばれたら再登録しとかないと次にコールバックされない）
  int cbid = sceKernelCreateCallback("Power Callback", power_callback);
  PowerSetCallback(0, cbid);
}

// ポーリング用スレッド
int CallbackThread(int args, void *argp)
{
  int cbid;
  
  // コールバック関数の登録
  cbid = sceKernelCreateCallback("Exit Callback", exit_callback);
  SetExitCallback(cbid);
  cbid = sceKernelCreateCallback("Power Callback", power_callback);
  PowerSetCallback(0, cbid);
  
  // ポーリング
  sceKernelPollCallbacks();
}

int SetupCallbacks(void)
{
  int thid = 0;
  
  // ポーリング用スレッドの生成
  thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
  if(thid >= 0)
    sceKernelStartThread(thid, 0, 0);
  
  return thid;
}

int xmain(int argc, char *argv)
{
  // Start Callbacks
  SetupCallbacks();

  // Init Memtable
  sceMemInit();
  
  // Init mp3inf
  minf.handle = 0;
  minf.fd = 0;
  minf.fsize = 0;
  
  // test play mp3 file
  mp3play("ms0:/PSP/MUSIC/01.mp3");
  
  return 0;
}

