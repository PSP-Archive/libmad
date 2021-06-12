// main.c
// ���C���Ƃ��X���b�h��Callback����̊֐�
// 

#include "main.h"

volatile int bSleep=0;

sceMemoryMgr MemTable[TABLE_SIZE];
MP3_INFO minf;

// �z�[���{�^���I�����ɃR�[���o�b�N
int exit_callback(void)
{
  scePowerSetClockFrequency(222,222,111);

  sceKernelExitGame();

  return 0;
}

// �X���[�v����s����ɃR�[���o�b�N
void power_callback(int unknown, int pwrflags)
{
  // �X���[�v���܂ރp���[�X�C�b�`OFF�������e��ۑ�����
  if(pwrflags & POWER_CB_POWER){
    bSleep=1;
    
    scePowerSetClockFrequency(222,222,111);
  }
  // �R�[���o�b�N�֐��̍ēo�^
  // �i��x�Ă΂ꂽ��ēo�^���Ƃ��Ȃ��Ǝ��ɃR�[���o�b�N����Ȃ��j
  int cbid = sceKernelCreateCallback("Power Callback", power_callback);
  PowerSetCallback(0, cbid);
}

// �|�[�����O�p�X���b�h
int CallbackThread(int args, void *argp)
{
  int cbid;
  
  // �R�[���o�b�N�֐��̓o�^
  cbid = sceKernelCreateCallback("Exit Callback", exit_callback);
  SetExitCallback(cbid);
  cbid = sceKernelCreateCallback("Power Callback", power_callback);
  PowerSetCallback(0, cbid);
  
  // �|�[�����O
  sceKernelPollCallbacks();
}

int SetupCallbacks(void)
{
  int thid = 0;
  
  // �|�[�����O�p�X���b�h�̐���
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

