#ifndef BEEP_H
#define BEEP_H

#include "Type.h"

enum CALCULATORMUSICTONE
{
    CMT_NULL = 255,
    CMT_1 = 0,
    CMT_2 = 2,
    CMT_3 = 3,
    CMT_4 = 5,
    CMT_5 = 7,
    CMT_6 = 9,
    CMT_7 = 11,
    CMT_8 = 12,
    CMT_9 = 14,
    CMT_P = 15,
    CMT_S = 17,
    CMT_M = 18,
    CMT_D = 21,
    CMT_E = 22,
};
struct TONE
{
    note_t note;
    uint8_t rp;
    uint16_t delay;
};
extern TONE testSound[];
extern TONE BootSound[];
extern TONE TipInstall[];
extern TONE TipRemove[];
extern TONE Beep1[];
extern TONE Beep2[];
extern TONE Beep3[];

extern uint32_t PlayTonesTimer;
extern uint16_t PlayTonesDelayTime;
extern uint16_t PlayTones_Schedule;
extern TONE* MySound;
//�ײ㺯��
void BeepInit(void);
void SetTone(double freq);
double GetNote(note_t note, uint8_t rp);
void SetNote(note_t note, uint8_t rp);
//�߲�Ӧ��
void SetSound(TONE sound[]);
void PlaySoundLoop(void);
uint8_t PlayTones(TONE sound[], uint16_t* Schedule);

#endif