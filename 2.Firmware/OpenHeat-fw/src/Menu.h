#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include "Type.h"

extern uint8_t Menu_System_State;
extern uint32_t pages_Tip_Display_timer;
extern uint8_t Menu_JumpAndExit;
extern uint8_t Menu_JumpAndExit_Level;

void System_TipMenu_Init(void);
void System_PIDMenu_Init(void);
void System_Menu_Init(void);
void System_UI_Init(void);
void System_UI(void);

int Get_Real_Menu_Level_Id(uint8_t id);
int Get_Menu_Id(uint8_t lid, uint8_t id);


void Menu_Control(void);
void Draw_APP(int x, int y, char* bitmap);
void Page_Footnotes(int a, int b);
void Text_Reader(char* s);
void Run_Menu_Id(uint8_t lid, uint8_t id);
void Next_Menu(void);
void Pop_Windows(char* s);
void Exit_Menu_System(void);
void Save_Exit_Menu_System(void);
void JumpWithTitle(void);
void SmoothAnimationSystem_Clean(void);

void Smooth_Animation_System(void);

void Update_OLED_Light_Level(void);
void Update_OLED_Flip(void);

void PopMsg_RotaryDirection(void);
void PopMsg_ScreenFlip(void);
void PopMsg_ListMode(void);
void SYS_About(void);

/*
	@名称 Smooth_Animation
	@变量
		x    过渡值
		last 上次的值
		val 目标值
		w    平滑权重
*/
struct Smooth_Animation
{
    float x;     //过渡值
    int last;     //上次的值
    int val;     //目标值
    float w;     //平滑权重
    uint8_t a;   //是否允许累加
    uint8_t b;   //选择过渡动画计算函数
};
extern struct Smooth_Animation Menu_Smooth_Animation[];
/*
	@名称 Slide_Bar
	@变量
		x    值
		min  最小值
		max  最大值
		step 步进
*/
struct Slide_Bar
{
    float* x;     //值
    float min;
    float max;
    float step;
};
extern struct Slide_Bar Slide_space[];

struct Menu_Level_System
{
    uint8_t id;   //当前层id
    int x;   //当前层所指选项
    uint8_t min;  //最小页
    uint8_t max;  //最大页
    uint8_t a;    //参数A 0:无动作 1:开启图标化菜单
};

struct Menu_System
{
    uint8_t lid;     //层id
    uint8_t id;      //选项id
    uint8_t x;       //执行操作 0-跳转到菜单 1-执行函数 2-菜单名 3-开关控件 4-滑动条控件
    char* name;  //选项名称
    uint8_t* icon;
    uint8_t a; //附加参数_1 (0-jump_Level_id) (3-开关id) (4-滑动条id)
    uint8_t b; //附加参数_2 (0-jump_id) (4-滑动条：true?执行函数:无操作)
    void (* function)();
};

extern struct Menu_System Menu[];
extern struct Menu_Level_System MenuLevel[];

#endif
