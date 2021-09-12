#include "OpenT12.h"

//菜单系统状态 0:自动退出 1:运行菜单系统
uint8_t Menu_System_State = false;
//跳转即退出菜单，该标志位适用于快速打开菜单设置，当遇到跳转操作时将保存设置并退出菜单
uint8_t Menu_JumpAndExit = false;
uint8_t Menu_JumpAndExit_Level = 255;   //当跳转完成后 的 菜单层级 等于“跳转即退出层级”时，“跳转即退出”立马生效

//菜单层id 菜单项目id
uint8_t MenuLevelId = 0, LastMenuLevelId = 0, MenuId = 0; //标注值
int32_t real_Level_Id, Pos_Id, Back_Id = -1; //索引值


//全局变量 控制右下角角标显示时间
uint32_t pages_Tip_Display_timer = 0;
//默认无动作后的1500ms关闭悬浮角标显示
#define pages_Tip_Display_Timeout 1500

/*/////////////////////////////////////////////////////////////////////

    @需要手动设置的项目

*//////////////////////////////////////////////////////////////////////
int MenuScroll = 0;


//由于sizeof的原理是在编译阶段由编译器替换数组大小，因此我们无法计算指针的sizeof！需要在位图的第一个字节填写 n 阶矩阵图像
/*
    @变量组 *Switch_space[]
    @brief 开关控件 用于存储开关控件的值
    @param -
    
    @注册
        -0->平滑动画
        -1->单选框测试
        -2->选项条固定长度
*/
enum Switch_space_Obj
{
    SwitchSpace_SmoothAnimation = 0,
    SwitchSpace_OptionStripFixedLength,

    SwitchSpace_PIDMode,
    SwitchSpace_KFP,
    SwitchSpace_PanelSettings,
    SwitchSpace_ScreenFlip,
    SwitchSpace_Volume,
    SwitchSpace_RotaryDirection,
    SwitchSpace_HandleTrigger,
    SwitchSpace_Language,
    SwitchSpace_TipID,

    SwitchSpace_BLE_State,
    SwitchSpace_MenuListMode,
};
uint8_t* Switch_space[] = {
    &SmoothAnimation_Flag,
    &OptionStripFixedLength_Flag,

    &PIDMode,
    &Use_KFP,
    &PanelSettings,
    &ScreenFlip,
    &Volume,
    &RotaryDirection,
    &HandleTrigger,
    &Language,
    &TipID,

    &BLE_State,
    &MenuListMode,
};

/*
    @结构体 Slide_Bar Slide_space[]
    @brief 滑动条控件 用于存储滑动条控件的值
    @param -
    
    @注册
        -0->屏幕亮度设定值
        -1->自适应菜单滚动范围
    @变量
        int   x    值
        int   min  最小值
        int   max  最大值
        int   step 步进
*/
enum Slide_space_Obj
{
    Slide_space_ScreenBrightness = 0,
    Slide_space_Scroll,

    Slide_space_BootTemp,
    Slide_space_SleepTemp,
    Slide_space_BoostTemp,

    Slide_space_ShutdownTime,
    Slide_space_SleepTime,
    Slide_space_BoostTime,
    Slide_space_ScreenProtectorTime,

    Slide_space_UndervoltageAlert,

    Slide_space_PID_AP,
    Slide_space_PID_AI,
    Slide_space_PID_AD,
    Slide_space_PID_CP,
    Slide_space_PID_CI,
    Slide_space_PID_CD,

    Slide_space_KFP_Q,
    Slide_space_KFP_R,

    Slide_space_SamplingRatioWork,
    Slide_space_ADC_PID_Cycle_List_0,
    Slide_space_ADC_PID_Cycle_List_1,
    Slide_space_ADC_PID_Cycle_List_2,

};
struct Slide_Bar Slide_space[] = {
    {(float*) &ScreenBrightness,      0,   255,  16}, //亮度调整
    {(float*) &MenuScroll,            0, SCREEN_ROW / 16, 1}, //自适应菜单滚动范围

    {(float*) &BootTemp,              150, 400,  5},
    {(float*) &SleepTemp,             150, 400,  5},
    {(float*) &BoostTemp,             0,   150,  1},

    {(float*) &ShutdownTime,          0,   60,   1},
    {(float*) &SleepTime,             0,   60,   1},
    {(float*) &BoostTime,             0,   600,  1},
    {(float*) &ScreenProtectorTime,   0,   600,  1},

    {(float*) &UndervoltageAlert,     0,   36,   0.25},

    {(float*) &aggKp,                 0,   50,   0.1},
    {(float*) &aggKi,                 0,   50,   0.1},
    {(float*) &aggKd,                 0,   50,   0.1},
    {(float*) &consKp,                0,   50,   0.1},
    {(float*) &consKi,                0,   50,   0.1},
    {(float*) &consKd,                0,   50,   0.1},

    {(float*) &KFP_Temp.Q,            0,   5,    0.01},
    {(float*) &KFP_Temp.R,            0,   25,   0.1},

    {(float*) &SamplingRatioWork,     1,   100,  1},
    {(float*) &ADC_PID_Cycle_List[0], 25,  2000, 25},
    {(float*) &ADC_PID_Cycle_List[1], 25,  2000, 25},
    {(float*) &ADC_PID_Cycle_List[2], 25,  2000, 25},
};

/*
    @结构体 Smooth_Animation Menu_Smooth_Animation[]
    @brief 存放过渡动画设定集
    @param -
    
    @变量
        float   x   过渡值
        int32_t     last 上次的值
        int32_t     val 当前值
        float   w   平滑权重
        uint8_t a   是否允许累加
        uint8_t b   选择过渡动画计算函数
*/
#define Smooth_Animation_Num sizeof(Menu_Smooth_Animation)/sizeof(Menu_Smooth_Animation[0])
struct Smooth_Animation Menu_Smooth_Animation[] = {
    {0, 0, 0, 0.4,  1, 0}, //菜单项目滚动动画
    {0, 0, 0, 0.15, 1, 0}, //滚动条平滑动画
    {0, 0, 0, 0.15, 1, 0}, //菜单选项条长度平滑动画
    {0, 0, 0, 0.2,  0, 0}, //项目归位动画
};


/*
    @结构体 Menu_Level_System MenuLevel[]
    @brief 菜单层级 设定
    @param -
    
    @变量
        uint8_t id    当前层id
        int x    当前层所指选项
        uint8_t min   最小页
        uint8_t max   最大页
        uint8_t a     参数A 0:无动作 1:开启图标化菜单
*/

#define Menu_NULL_IMG 0
#define Menu_HAVE_IMG 1

struct Menu_Level_System MenuLevel[] = {
    {0,  0, 0, 3,            Menu_NULL_IMG},
    {1,  0, 0, 5,            Menu_HAVE_IMG},
    {2,  0, 0, 8,            Menu_HAVE_IMG},
    {3,  0, 0, 4,            Menu_HAVE_IMG},
    {4,  0, 0, 5,            Menu_HAVE_IMG},
    {5,  0, 0, 7,            Menu_HAVE_IMG},
    {6,  0, 0, 5,            Menu_HAVE_IMG},
    {7,  0, 0, 7,            Menu_HAVE_IMG},
    {8,  0, 0, 2,            Menu_HAVE_IMG},
    {9,  0, 0, 9,            Menu_NULL_IMG},
    {10, 0, 0, 2,            Menu_HAVE_IMG},
    {11, 0, 0, 2,            Menu_HAVE_IMG},
    {12, 0, 0, 2,            Menu_HAVE_IMG},
    {13, 0, 0, 1,            Menu_HAVE_IMG},
    {14, 0, 0, 2,            Menu_HAVE_IMG},

    {15, 0, 0, MaxTipConfig, Menu_NULL_IMG},

    {16, 0, 0, 3,            Menu_NULL_IMG},
    {17, 0, 0, 4,            Menu_NULL_IMG},
    {18, 0, 0, 4,            Menu_NULL_IMG},
    {19, 0, 0, 5,            Menu_NULL_IMG},
    {20, 0, 0, 4,            Menu_NULL_IMG},
    {21, 0, 0, 4,            Menu_NULL_IMG},
    {22, 0, 0, 3,            Menu_NULL_IMG},
};

/*
    @结构体 Menu_System Menu[]
    @brief 菜单层级项目 设定
    @param -
    
    @变量
        uint8_t lid             层id
        uint8_t id              选项id
        uint8_t x               执行操作 0-跳转到菜单 1-执行函数 2-菜单名 3-开关控件 4-滑动条控件
        char name[21]      选项名称 支持半角
        char *icon         图标(此项选填 若当前层级菜单开启了图标化) 详细可以参考关于图标化的说明文档
        uint8_t a               附加参数_1 (0-jump_Level_id) (3-开关id) (4-滑动条id)
        uint8_t b               附加参数_2 (0-jump_id) (4-滑动条：true?执行函数:无操作)
        void (*function)(void);
*/
#define Menu_NULL_F                 0

#define Jump_Menu_Op                0
#define F_Menu_Op                   1
#define Title_Menu_Op               2
#define Switch_Menu_Op              3
#define Progress_Bar_Menu_Op        4
#define SingleBox_Menu_Op           5
#define Menu_NULL_OP                6

struct Menu_System Menu[] = {
    {0,  0,  Title_Menu_Op,        "[控制器设置]", Menu_NULL_IMG,     0,                                  0,            *Save_Exit_Menu_System},
    {0,  1,  Jump_Menu_Op,         "此热台",         Menu_NULL_IMG,     1,                                  0,     Menu_NULL_F},
    {0,  2,  Jump_Menu_Op,         "此系统",         Menu_NULL_IMG,     5,                                  0,     Menu_NULL_F},
    {0,  3,  F_Menu_Op,            "返回",          Menu_NULL_IMG,     0,                                  0,            *Save_Exit_Menu_System},
    // { 0,4,       F_Menu_Op,          "测试",               Menu_NULL_IMG,                0,                                  0,          *(EnterLogo)},
    // { 0,4,       F_Menu_Op,          "重启",               Menu_NULL_IMG,                0,                                  0,          *(SYS_Reboot)},

    {1,  0,  Title_Menu_Op,        "此热台",         Menu_NULL_IMG,     0,                                  1,     Menu_NULL_F},
    {1,  1,  Jump_Menu_Op,         "烙铁头",     IMG_Tip,               2,                                  0,     Menu_NULL_F},
    {1,  2,  Jump_Menu_Op,         "温度场景",    Set1,                  3,                                  0,     Menu_NULL_F},
    {1,  3,  Jump_Menu_Op,         "定时场景",    Set2,                  4,                                  0,     Menu_NULL_F},
    {1,  4,  Jump_Menu_Op,         "温控设置",    Set3,                  19,                                 0,     Menu_NULL_F},
    {1,  5,  Jump_Menu_Op,         "返回",      Set7,                  0,                                  1,     Menu_NULL_F},

    {2,  0,  Title_Menu_Op,        "烙铁头管理",       Menu_NULL_IMG,     1,                                  1,     Menu_NULL_F},
    {2,  1,  Jump_Menu_Op,         "切换配置",    Set8,                  15,                                 0,            *FlashTipMenu},
    {2,  2,  F_Menu_Op,            "查看温度曲线",  Set0,                  0,                                  0,            *ShowCurveCoefficient},
    {2,  3,  F_Menu_Op,            "校准温度",    Set9,                  0,                                  0,            *CalibrationTemperature},
    {2,  4,  Jump_Menu_Op,         "PID参数",   Set3,                  16,                                 0,     Menu_NULL_F},
    {2,  5,  F_Menu_Op,            "新建",      IMG_Files,             0,                                  0,            *NewTipConfig},
    {2,  6,  F_Menu_Op,            "重命名",     IMG_Pen2,              0,                                  0,            *TipRename},
    {2,  7,  F_Menu_Op,            "删除",      Set10,                 0,                                  0,            *TipDel},
    {2,  8,  Jump_Menu_Op,         "返回",      Save,                  1,                                  1,            *SaveTipConfig},

    {3,  0,  Title_Menu_Op,        "温度场景",        Menu_NULL_IMG,     1,                                  2,     Menu_NULL_F},
    {3,  1,  Progress_Bar_Menu_Op, "启动温度",    Set13,                 Slide_space_BootTemp,               0,     Menu_NULL_F},
    {3,  2,  Progress_Bar_Menu_Op, "提温温度",    Set14,                 Slide_space_BoostTemp,              0,     Menu_NULL_F},
    {3,  3,  Progress_Bar_Menu_Op, "休眠温度",    Set11,                 Slide_space_SleepTemp,              0,     Menu_NULL_F},
    {3,  4,  Jump_Menu_Op,         "返回",      Save,                  1,                                  2,     Menu_NULL_F},

    {4,  0,  Title_Menu_Op,        "定时场景",        Menu_NULL_IMG,     1,                                  3,     Menu_NULL_F},
    {4,  1,  Progress_Bar_Menu_Op, "停机触发(分)", Set13,                 Slide_space_ShutdownTime,           0,     Menu_NULL_F},
    {4,  2,  Progress_Bar_Menu_Op, "提温时长(秒)", Set14,                 Slide_space_BoostTime,              0,     Menu_NULL_F},
    {4,  3,  Progress_Bar_Menu_Op, "休眠触发(分)", Set11,                 Slide_space_SleepTime,              0,     Menu_NULL_F},
    {4,  4,  Progress_Bar_Menu_Op, "屏保触发(秒)", Set4,                  Slide_space_ScreenProtectorTime,    0,     Menu_NULL_F},
    {4,  5,  Jump_Menu_Op,         "返回",      Save,                  1,                                  3,     Menu_NULL_F},

    {5,  0,  Title_Menu_Op,        "此系统",         Menu_NULL_IMG,     0,                                  2,     Menu_NULL_F},
    {5,  1,  Jump_Menu_Op,         "个性化",     IMG_Pen,               6,                                  0,     Menu_NULL_F},
    {5,  2,  Jump_Menu_Op,         "蓝牙",      IMG_BLE,               22,                                 0,     Menu_NULL_F},
    {5,  3,  Progress_Bar_Menu_Op, "欠压提醒",    Set6,                  Slide_space_UndervoltageAlert,      0,     Menu_NULL_F},
    {5,  4,  F_Menu_Op,            "开机密码",    Lock,                  0,                                  0,            *SetPasswd},
    {5,  5,  Jump_Menu_Op,         "语言设置",    Set_LANG,              13,                                 0,     Menu_NULL_F},
    {5,  6,  F_Menu_Op,            "关于朱雀",    QRC,                   5,                                  5,            *About},
    {5,  7,  Jump_Menu_Op,         "返回",      Set7,                  0,                                  2,     Menu_NULL_F},

    {6,  0,  Title_Menu_Op,        "个性化",         Menu_NULL_IMG,     5,                                  1,     Menu_NULL_F},
    {6,  1,  Jump_Menu_Op,         "显示效果",    Set4,                  7,                                  0,     Menu_NULL_F},
    {6,  2,  Jump_Menu_Op,         "声音设置",    Set5,                  10,                                 0,     Menu_NULL_F},
    {6,  3,  Switch_Menu_Op,       "编码器方向",   Set19,                 SwitchSpace_RotaryDirection,        0,            *PopMsg_RotaryDirection},
    {6,  4,  Jump_Menu_Op,         "手柄触发",    IMG_Trigger,           14,                                 0,     Menu_NULL_F},
    {6,  5,  Jump_Menu_Op,         "返回",      Set7,                  5,                                  1,     Menu_NULL_F},

    {7,  0,  Title_Menu_Op,        "显示效果",        Menu_NULL_IMG,     6,                                  1,     Menu_NULL_F},
    {7,  1,  Jump_Menu_Op,         "面板设置",    Set0,                  8,                                  0,     Menu_NULL_F},
    {7,  2,  Switch_Menu_Op,       "翻转屏幕",    IMG_Flip,              SwitchSpace_ScreenFlip,             0,            *Update_OLED_Flip},
    {7,  3,  Jump_Menu_Op,         "过渡动画",    IMG_Animation,         11,                                 0,     Menu_NULL_F},
    {7,  4,  Progress_Bar_Menu_Op, "屏幕亮度",    IMG_Sun,               Slide_space_ScreenBrightness,       1,            *Update_OLED_Light_Level},
    {7,  5,  Jump_Menu_Op,         "选项条定宽",   IMG_Size,              9,                                  0,     Menu_NULL_F},
    {7,  6,  Switch_Menu_Op,       "列表模式",    IMG_ListMode,          SwitchSpace_MenuListMode,           0,            *PopMsg_ListMode},
    {7,  7,  Jump_Menu_Op,         "返回",      Set7,                  6,                                  1,     Menu_NULL_F},

    {8,  0,  Title_Menu_Op,        "面板设置",        Menu_NULL_IMG,     7,                                  1,     Menu_NULL_F},
    {8,  1,  SingleBox_Menu_Op,    "简约",      Set17,                 SwitchSpace_PanelSettings,          0,            *JumpWithTitle},
    {8,  2,  SingleBox_Menu_Op,    "详细",      Set18,                 SwitchSpace_PanelSettings,          1,            *JumpWithTitle},

    {9,  0,  Title_Menu_Op,        "选项条定宽设置&测试",  Menu_NULL_IMG,     7,                                  5,     Menu_NULL_F},
    {9,  1,  SingleBox_Menu_Op,    "固定",          Menu_NULL_IMG,     SwitchSpace_OptionStripFixedLength, true,  Menu_NULL_F},
    {9,  2,  SingleBox_Menu_Op,    "自适应",         Menu_NULL_IMG,     SwitchSpace_OptionStripFixedLength, false, Menu_NULL_F},
    {9,  3,  Jump_Menu_Op,         "--- 往下翻 ---", Menu_NULL_IMG,     9,                                  4,     Menu_NULL_F},
    {9,  4,  Menu_NULL_OP,         "人民!",         Menu_NULL_IMG,     0,                                  0,     Menu_NULL_F},
    {9,  5,  Menu_NULL_OP,         "只有人民~",       Menu_NULL_IMG,     0,                                  0,     Menu_NULL_F},
    {9,  6,  Menu_NULL_OP,         "才是创造世界历史的",   Menu_NULL_IMG,     0,                                  1,     Menu_NULL_F},
    {9,  7,  Menu_NULL_OP,         "动 力！",        Menu_NULL_IMG,     0,                                  0,     Menu_NULL_F},
    {9,  8,  Jump_Menu_Op,         "--- 往上翻 ---", Menu_NULL_IMG,     9,                                  0,     Menu_NULL_F},
    {9,  9,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     7,                                  5,     Menu_NULL_F},

    {10, 0,  Title_Menu_Op,        "声音设置",        Menu_NULL_IMG,     6,                                  2,     Menu_NULL_F},
    {10, 1,  SingleBox_Menu_Op,    "开启",      Set5,                  SwitchSpace_Volume,                 true,         *JumpWithTitle},
    {10, 2,  SingleBox_Menu_Op,    "关闭",      Set5_1,                SwitchSpace_Volume,                 false,        *JumpWithTitle},

    {11, 0,  Title_Menu_Op,        "动画设置",        Menu_NULL_IMG,     7,                                  3,     Menu_NULL_F},
    {11, 1,  SingleBox_Menu_Op,    "开启",      IMG_Animation,         SwitchSpace_SmoothAnimation,        true,         *JumpWithTitle},
    {11, 2,  SingleBox_Menu_Op,    "关闭",      IMG_Animation_DISABLE, SwitchSpace_SmoothAnimation,        false,        *JumpWithTitle},

    {12, 0,  Title_Menu_Op,        "温控模式",        Menu_NULL_IMG,     19,                                 2,     Menu_NULL_F},
    {12, 1,  SingleBox_Menu_Op,    "PID控制",   Set16,                 SwitchSpace_PIDMode,                true,         *JumpWithTitle},
    {12, 2,  SingleBox_Menu_Op,    "模糊控制",    Set15,                 SwitchSpace_PIDMode,                false,        *JumpWithTitle},

    {13, 0,  Title_Menu_Op,        "语言设置",        Menu_NULL_IMG,     5,                                  5,     Menu_NULL_F},
    {13, 1,  SingleBox_Menu_Op,    "简体中文",    Lang_CN,               SwitchSpace_Language,               LANG_Chinese, *JumpWithTitle},

    {14, 0,  Title_Menu_Op,        "手柄触发",        Menu_NULL_IMG,     6,                                  4,     Menu_NULL_F},
    {14, 1,  SingleBox_Menu_Op,    "震动开关",    IMG_VibrationSwitch,   SwitchSpace_HandleTrigger,          0,            *JumpWithTitle},
    {14, 2,  SingleBox_Menu_Op,    "干簧管",     IMG_ReedSwitch,        SwitchSpace_HandleTrigger,          1,            *JumpWithTitle},

    {15, 0,  Title_Menu_Op,        "烙铁头列表",       Menu_NULL_IMG,     2,                                  1,            *LoadTipConfig},
    {15, 1,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  0,            *JumpWithTitle},
    {15, 2,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  1,            *JumpWithTitle},
    {15, 3,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  2,            *JumpWithTitle},
    {15, 4,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  3,            *JumpWithTitle},
    {15, 5,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  4,            *JumpWithTitle},
    {15, 6,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  5,            *JumpWithTitle},
    {15, 7,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  6,            *JumpWithTitle},
    {15, 8,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  7,            *JumpWithTitle},
    {15, 9,  SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  8,            *JumpWithTitle},
    {15, 10, SingleBox_Menu_Op,    "",            Menu_NULL_IMG,     SwitchSpace_TipID,                  9,            *JumpWithTitle},

    {16, 0,  Title_Menu_Op,        "PID参数",       Menu_NULL_IMG,     2,                                  3,     Menu_NULL_F},
    {16, 1,  Jump_Menu_Op,         "PID爬升期参数",    Menu_NULL_IMG,     17,                                 0,     Menu_NULL_F},
    {16, 2,  Jump_Menu_Op,         "PID接近期参数",    Menu_NULL_IMG,     18,                                 0,     Menu_NULL_F},
    {16, 3,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     2,                                  3,     Menu_NULL_F},

    {17, 0,  Title_Menu_Op,        "PID爬升期",      Menu_NULL_IMG,     16,                                 1,            *SaveTipConfig},
    {17, 1,  Progress_Bar_Menu_Op, "比例P",         Menu_NULL_IMG,     Slide_space_PID_AP,                 0,     Menu_NULL_F},
    {17, 2,  Progress_Bar_Menu_Op, "积分I",         Menu_NULL_IMG,     Slide_space_PID_AI,                 0,     Menu_NULL_F},
    {17, 3,  Progress_Bar_Menu_Op, "微分D",         Menu_NULL_IMG,     Slide_space_PID_AD,                 0,     Menu_NULL_F},
    {17, 4,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     16,                                 1,            *SaveTipConfig},

    {18, 0,  Title_Menu_Op,        "PID接近期",      Menu_NULL_IMG,     16,                                 2,            *SaveTipConfig},
    {18, 1,  Progress_Bar_Menu_Op, "比例P",         Menu_NULL_IMG,     Slide_space_PID_CP,                 0,     Menu_NULL_F},
    {18, 2,  Progress_Bar_Menu_Op, "积分I",         Menu_NULL_IMG,     Slide_space_PID_CI,                 0,     Menu_NULL_F},
    {18, 3,  Progress_Bar_Menu_Op, "微分D",         Menu_NULL_IMG,     Slide_space_PID_CD,                 0,     Menu_NULL_F},
    {18, 4,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     16,                                 2,            *SaveTipConfig},

    {19, 0,  Title_Menu_Op,        "温控设置",        Menu_NULL_IMG,     1,                                  4,     Menu_NULL_F},
    {19, 1,  Switch_Menu_Op,       "PID状态",       Menu_NULL_IMG,     SwitchSpace_PIDMode,                0,     Menu_NULL_F},
    {19, 2,  Progress_Bar_Menu_Op, "采样/加热 %",     Menu_NULL_IMG,     Slide_space_SamplingRatioWork,      0,     Menu_NULL_F},
    {19, 3,  Jump_Menu_Op,         "采样周期(ms)",    Menu_NULL_IMG,     21,                                 0,     Menu_NULL_F},
    {19, 4,  Jump_Menu_Op,         "卡尔曼滤波器",      Menu_NULL_IMG,     20,                                 0,     Menu_NULL_F},
    {19, 5,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     1,                                  4,     Menu_NULL_F},

    {20, 0,  Title_Menu_Op,        "卡尔曼滤波器",      Menu_NULL_IMG,     19,                                 4,     Menu_NULL_F},
    {20, 1,  Switch_Menu_Op,       "启用状态",        Menu_NULL_IMG,     SwitchSpace_KFP,                    0,     Menu_NULL_F},
    {20, 2,  Progress_Bar_Menu_Op, "过程噪声协方差",     Menu_NULL_IMG,     Slide_space_KFP_Q,                  0,     Menu_NULL_F},
    {20, 3,  Progress_Bar_Menu_Op, "观察噪声协方差",     Menu_NULL_IMG,     Slide_space_KFP_R,                  0,     Menu_NULL_F},
    {20, 4,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     19,                                 4,     Menu_NULL_F},

    {21, 0,  Title_Menu_Op,        "采样周期",        Menu_NULL_IMG,     19,                                 3,     Menu_NULL_F},
    {21, 1,  Progress_Bar_Menu_Op, "温差>150",      Menu_NULL_IMG,     Slide_space_ADC_PID_Cycle_List_0,   0,     Menu_NULL_F},
    {21, 2,  Progress_Bar_Menu_Op, "温差>50",       Menu_NULL_IMG,     Slide_space_ADC_PID_Cycle_List_1,   0,     Menu_NULL_F},
    {21, 3,  Progress_Bar_Menu_Op, "温差≤50",       Menu_NULL_IMG,     Slide_space_ADC_PID_Cycle_List_2,   0,     Menu_NULL_F},
    {21, 4,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     19,                                 3,     Menu_NULL_F},

    {22, 0,  Title_Menu_Op,        "蓝牙",          Menu_NULL_IMG,     5,                                  2,     Menu_NULL_F},
    {22, 1,  Switch_Menu_Op,       "状态",          Menu_NULL_IMG,     SwitchSpace_BLE_State,              0,            *BLE_Restart},
    {22, 2,  F_Menu_Op,            "设备名称",        Menu_NULL_IMG,     22,                                 2,            *BLE_Rename},
    {22, 3,  Jump_Menu_Op,         "返回",          Menu_NULL_IMG,     5,                                  2,     Menu_NULL_F},
};
/***
 * @description: 快速打开烙铁列表
 * @param {*}
 * @return {*}
 */
void System_TipMenu_Init(void)
{
    printf("尝试打开烙铁头列表\n");
    //关闭功率管输出
    SetPOWER(0);
    //初始化菜单
    FlashTipMenu();                 //刷新菜单系统烙铁列表

    Menu_JumpAndExit = true;   //菜单标志：“跳转即退出” 在设置完Tip后自动退出菜单
    Menu_JumpAndExit_Level = 2; //当菜单进行跳转操作，跳转到该 Menu_JumpAndExit_Level 层后检查“跳转即退出” 标志

    MenuLevel[15].x = 0;  //复位第一层菜单的位置
    MenuLevelId = 15;       //设定跳转目标
    *Slide_space[Slide_space_Scroll].x = 0;//复位第一层菜单的位置
    Next_Menu();
}
/*** 
 * @description: 快速打开PID菜单
 * @param {*}
 * @return {*}
 */
void System_PIDMenu_Init(void)
{
    printf("尝试打开PID菜单\n");
    //关闭功率管输出
    SetPOWER(0);
    //初始化菜单
    FlashTipMenu();                 //刷新菜单系统烙铁列表

    Menu_JumpAndExit = true;   //菜单标志：“跳转即退出” 在设置完Tip后自动退出菜单
    Menu_JumpAndExit_Level = 2; //当菜单进行跳转操作，跳转到该 Menu_JumpAndExit_Level 层后检查“跳转即退出” 标志

    MenuLevel[16].x = 0;  //复位第一层菜单的位置
    MenuLevelId = 16;       //设定跳转目标
    *Slide_space[Slide_space_Scroll].x = 0;//复位第一层菜单的位置
    Next_Menu();
    printf("菜单状态:%d\n", Menu_System_State);
}

/*** 
 * @description: 初始化菜单
 * @param {*}
 * @return {*}
 */
void System_Menu_Init(void)
{
    //关闭功率管输出
    SetPOWER(0);
    //关闭输出
    PWMOutput_Lock = true;
    SetPOWER(0);
    //初始化菜单
    MenuLevel[0].x = 0;  //复位第一层菜单的位置
    MenuLevelId = 0;       //设定跳转目标
    *Slide_space[Slide_space_Scroll].x = 0;//复位第一层菜单的位置
    Next_Menu();

    //解除编码器锁定（如果有）
    Counter_LOCK_Flag = false;
}
/*** 
 * @description: 初始化主界面
 * @param {*}
 * @return {*}
 */
void System_UI_Init(void)
{
    sys_Counter_Set(TipMinTemp, TipMaxTemp, 5, PID_Setpoint);
    //输出解锁
    PWMOutput_Lock = false;
}
//系统UI
void System_UI(void)
{
    Clear();

    if (Menu_System_State)
    {
        Menu_Control();
    } else
    {
        //睡眠模式屏保入口
        if (SleepEvent && SleepScreenProtectFlag)
            RunSleepLoop();
        else
        {
            // char buffer[50];
            // for (uint8_t i = 0;i < 5;i++) {
            //     Disp.setCursor(0, 12 * i + 1);

            //     switch (i) {
            //     case 0: sprintf(buffer, "状态%d:%s 控温:%s", TempCTRL_Status, TempCTRL_Status_Mes[TempCTRL_Status], (PIDMode == 1) ? "PID" : "模糊"); break;
            //     case 1: sprintf(buffer, "设定%.0lf°C 当前%.0lf°C", PID_Setpoint, TipTemperature); break;
            //     case 2: sprintf(buffer, "ADC:%d PID:%.0lf", LastADC, PID_Output); break;
            //     case 3: sprintf(buffer, "E:%.2lf V2:%.2lf", Get_MainPowerVoltage(), ESP32_ADC2Vol(analogRead(POWER_ADC_PIN))); break;
            //     case 4: sprintf(buffer, "%.3lf %.3lf %.3lf", aggKp, aggKi, aggKd); break;
            //     }
            //     Disp.print(buffer);
            // }

            ///////////////////////////////////////////////////////////////////////////////////

            //显示烙铁头名称
            Disp.drawUTF8(0, 1, TipName);

            //温度控制状态图标
            Draw_Slow_Bitmap(74, 37, C_table[TempCTRL_Status], 14, 14);
            //显示中文状态信息
            Disp.drawUTF8(91, 40, TempCTRL_Status_Mes[TempCTRL_Status]);

            //欠压警报
            if (UnderVoltageEvent)
            {
                if ((millis() / 1000) % 2)
                {
                    //欠压告警图标
                    Draw_Slow_Bitmap(74, 21, Battery_NoPower, 14, 14);
                } else
                {
                    //主电源电压
                    Disp.setCursor(74, 24);
                    Disp.printf("%.1fV", Get_MainPowerVoltage());
                }
            } else
            {
                //显示蓝牙图标
                if (BLE_State) Draw_Slow_Bitmap(92, 25, IMG_BLE_S, 9, 11);
            }

            ///////////////////////////////////////////////////////////////////////////////////
            //显示当前温度
            Disp.setFont(u8g2_font_logisoso38_tr);
            Disp.setCursor(0, 12);

            if (TempCTRL_Status == TEMP_STATUS_ERROR)
            {
                if ((millis() / 250) % 2) Disp.print("---");
            } else
            {
                //如果温度波动足够小，则显示当前温度为设定温度
                if (TempGap < 10) Disp.printf("%.0lf", PID_Setpoint);  //显示"假"温度(设定温度)
                else Disp.printf("%.0lf", TipTemperature);    //显示真实温度
            }

            Disp.setFont(u8g2_font_wqy12_t_gb2312);
            ///////////////////////////////////////////////////////////////////////////////////

            //右上角运行指示角标
            if (POWER > 0 && PWM_WORKY)
            {
                uint8_t TriangleSize = map(POWER, 0, 255, 16, 0);
                //Disp.drawTriangle(100 + TriangleSize, 0, 127, 0, 127, 27 - TriangleSize);
                Disp.drawTriangle((119 - 12) + TriangleSize, 12, 125, 12, 125, (18 + 12) - TriangleSize);
                // Draw_Slow_Bitmap(114, 15, PositioningCursor, 8, 8);
                //Disp.drawTriangle(103, 0, 127, 0, 127, 24);
            }



            /////////////////////////////////////绘制遮罩层//////////////////////////////////////////////
            Disp.setDrawColor(2);
            //几何图形切割
            Disp.drawBox(0, 12, 96, 40);
            Disp.drawTriangle(96, 12, 96, 52, 125, 42);
            Disp.drawTriangle(125, 42, 96, 52, 118, 52);
            Disp.setDrawColor(1);

            //绘制底部状态条
            DrawStatusBar(1);

            //如果当前是处于爆发技能，则显示技能剩余时间进度条
            if (TempCTRL_Status == TEMP_STATUS_BOOST && DisplayFlashTick % 2)
            {
                uint8_t BoostTimeBar = map(millis() - BoostTimer, 0, BoostTime * 1000, 0, 14);
                Disp.drawBox(74, 37, 14, BoostTimeBar);

            }

        }
        Display();
        //编码器长按按键进入菜单
        if (SYSKey == 2)
        {
            //初始化菜单
            System_Menu_Init();
        }
    }

}

/*/////////////////////////////////////////////////////////////////////

    @自定义功能函数

*//////////////////////////////////////////////////////////////////////
/*
    @函数 Update_OLED_Light_Level
    @brief 更新屏幕亮度设置
    @param -

*/
void Update_OLED_Light_Level(void)
{
    Disp.sendF("c", 0x81);  //向SSD1306发送指令：设置内部电阻微调
    Disp.sendF("c", (uint8_t) *Slide_space[Slide_space_ScreenBrightness].x); //微调范围（0-255）
}

void Update_OLED_Flip(void)
{
    // ToDo fix u8g2 oled-12880 flip error
//    Disp.setFlipMode(ScreenFlip);
    if (Menu_System_State) PopMsg_ScreenFlip();
}

void PopMsg_RotaryDirection(void)
{
    char buffer[20];
    sprintf(buffer, "编码器:%s", (RotaryDirection == true) ? "顺时针" : "逆时针");
    Pop_Windows(buffer);
    delay(500);
}

void PopMsg_ScreenFlip(void)
{
    char buffer[20];
    sprintf(buffer, "%s", (ScreenFlip == true) ? "翻转显示" : "正常显示");
    Pop_Windows(buffer);
    delay(500);
}

void PopMsg_ListMode(void)
{
    char buffer[20];
    sprintf(buffer, "%s", (MenuListMode == true) ? "列表模式" : "图标模式");
    Pop_Windows(buffer);
    delay(500);
    Next_Menu();
}


/*/////////////////////////////////////////////////////////////////////

    @相关工具函数

*//////////////////////////////////////////////////////////////////////
#define RollingStripWidth 3

/*复选框选中 10*10*/
uint8_t CheckBoxSelection[] = {0xff, 0xc0, 0x80, 0x40, 0x80, 0xc0, 0x81, 0xc0, 0x81, 0xc0, 0x83, 0x40, 0x9b,
                               0x40, 0x8e, 0x40, 0x86, 0x40, 0xff, 0xc0};

void Save_Exit_Menu_System(void)
{
    //过渡离开
    Disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 66 * *Switch_space[SwitchSpace_SmoothAnimation]);
    Disp.setDrawColor(1);

    //保存配置
    SYS_Save();

    Exit_Menu_System();
}

void Exit_Menu_System(void)
{
    printf("退出菜单系统\n");
    Menu_System_State = 0;
    Menu_JumpAndExit = false;
    Menu_JumpAndExit_Level = 255;

    //退出菜单后重新初始化主界面
    System_UI_Init();
}

//按照标题进行跳转 标题跳转 跳转标题
void JumpWithTitle(void)
{
    Run_Menu_Id(MenuLevelId, 0);
}

/*
    @函数 Smooth_Animation_System
    @brief 过渡动画运算
*/
void Smooth_Animation_System()
{
    for (uint8_t i = 0; i < Smooth_Animation_Num; i++)
    {
        //优化计算：变形过滤器
        if (Menu_Smooth_Animation[i].x && abs(Menu_Smooth_Animation[i].x * 100) < 1.5)
            Menu_Smooth_Animation[i].x = 0;
        //动画控制变量是否需要更新
        if (Menu_Smooth_Animation[i].last != Menu_Smooth_Animation[i].val)
        {
            //是否允许累加
            if (Menu_Smooth_Animation[i].a)
                Menu_Smooth_Animation[i].x += Menu_Smooth_Animation[i].val - Menu_Smooth_Animation[i].last;
            else Menu_Smooth_Animation[i].x = Menu_Smooth_Animation[i].val - Menu_Smooth_Animation[i].last;
            //重置标志
            Menu_Smooth_Animation[i].last = Menu_Smooth_Animation[i].val;
            //动画特殊个性化配置
            switch (i)
            {
                default:
                    break;
            }
        }
        //使用被选的动画计算函数计算动画
        switch (Menu_Smooth_Animation[i].b)
        {
            case 0:
                Menu_Smooth_Animation[i].x -= Menu_Smooth_Animation[i].x * Menu_Smooth_Animation[i].w;
                break;
        }
    }
}
void SmoothAnimationSystem_Clean()
{
    for (uint8_t i = 0; i < Smooth_Animation_Num; i++)
    {
        Menu_Smooth_Animation[i].x = 0;
    }
}
/*
    @函数 Page_Footnotes
    @brief 自适应屏幕右下角角标绘制
    @param int a 分子
    @param int b 分母
*/
void Page_Footnotes(int a, int b)
{
    char buffer[20];
    uint8_t w = (Get_Dec_Deep(a) + Get_Dec_Deep(b) + 3) * 6;
    uint8_t x = SCREEN_COLUMN - 8 - w;
    uint8_t y = SCREEN_ROW - 12;

    if (millis() < pages_Tip_Display_timer + pages_Tip_Display_Timeout)
    {
        //绘制白色底色块
        Disp.setDrawColor(1);
        Disp.drawRBox(x + 1, y - 1, w, 13, 1);
        //绘制下标文字
        Disp.setDrawColor(0);
        Disp.setCursor(x, y + 1);
        sprintf(buffer, "[%d/%d]", a, b);
        Disp.print(buffer);
    }
    //恢复颜色设置
    Disp.setDrawColor(1);
}

/*
    @函数 Text_Reader
    @brief 文本浏览器（全角GBK）
    @param (char* s):字符串首地址
    
*/
void Text_Reader(char* s)
{
    int len = strlen(s) / 2;
    int Colunm_GBK_Char = SCREEN_COLUMN / CNSize;
    int Row_GBK_Char = SCREEN_ROW / CNSize;
    int pages = len / (Colunm_GBK_Char * Row_GBK_Char);
    int val_Pages = 0, last_Pages = pages + 1;
    char page_s[(SCREEN_COLUMN / CNSize) * 2 + 1];
    int now_pow;

    //重置页码角标显示时间
    pages_Tip_Display_timer = millis();

    //初始化编码器设置
    sys_Counter_Set(0, pages, 1, 0);
    while (!sys_KeyProcess())
    {

        //获取输入
        val_Pages = sys_Counter_Get();
        //只有翻页时才刷新屏幕 节省资源::这里页码超时加了100ms是为了超时后可以Display刷新一下屏幕
        if (val_Pages != last_Pages || (millis() - pages_Tip_Display_timer < pages_Tip_Display_Timeout + 100))
        {
            last_Pages = val_Pages;
            Disp.clearBuffer();

            //自适应屏幕高度
            for (int i = 0; i < Row_GBK_Char; i++)
            {
                //计算当前行第一个字符地址
                now_pow = (val_Pages * Colunm_GBK_Char * Row_GBK_Char + Colunm_GBK_Char * i) * 2;
                for (int i = 0; i < sizeof(page_s); i++)
                {
                    if (now_pow + i < strlen(s) && i < sizeof(page_s) - 1)
                        page_s[i] = s[now_pow + i];
                    else
                    {
                        page_s[i] = '\0';
                        break;
                    }
                }
                //显示当前行Utf8字符
                Draw_Utf(0, i * (CNSize + 1), page_s);
            }
            //绘制滚动条
            Draw_Scale(SCREEN_COLUMN - 5, 0, 5, SCREEN_ROW - 1, pages + 1,
                       map(val_Pages, 0, pages, 0, SCREEN_ROW - SCREEN_ROW / 2 - 1));
            //绘制页码下标
            Page_Footnotes(val_Pages + 1, pages + 1);
            Display();
        }
        //ESP.wdtFeed();
    }
    //延迟 防止短时间多次触发
    delay(50);
    //初始化菜单控件
    //Next_Menu();
}

/*
    @函数 Get_Real_Menu_Level_Id
    @brief 获取 菜单层对象 的索引值
    @param uint8_t id 菜单层对象id
    @return 菜单层对象 的索引值
*/
int Get_Real_Menu_Level_Id(uint8_t id)
{
    uint8_t real_Level_Id;
    for (uint8_t i = 0; i < sizeof(MenuLevel) / sizeof(MenuLevel[0]); i++)
        if (MenuLevel[i].id == id) real_Level_Id = i;
    return real_Level_Id;
}

/*
    @函数 Get_Menu_Id
    @brief 获取 菜单对象 的索引值
    @param 菜单层对象id 菜单对象id
    @return 菜单对象 的索引值
*/
int Get_Menu_Id(uint8_t lid, uint8_t id)
{
    uint8_t real_Level_Id;
    for (uint8_t i = 0; i < sizeof(Menu) / sizeof(Menu[0]); i++)
        if (Menu[i].lid == lid && Menu[i].id == id) real_Level_Id = i;
    return real_Level_Id;
}

/*** 
 * @description: 菜单系统设置编码器
 * @param {*}
 * @return {*}
 */
void MenuSYS_SetCounter()
{
    if (!Menu_System_State) return;
    printf("菜单系统设置编码器\n");
    if (!MenuLevel[real_Level_Id].a || MenuListMode || SCREEN_ROW <= 32)
    {
        //设置编码器滚动范围
        MenuLevel[real_Level_Id].min = 0;   //重置选项最小值：从图标模式切换到列表模式会改变该值
        uint8_t MinimumScrolling = min((int) Slide_space[Slide_space_Scroll].max,
                                       (int) MenuLevel[real_Level_Id].max);
        sys_Counter_Set((int) Slide_space[Slide_space_Scroll].min, MinimumScrolling + 1, 1,
                        (int) *Slide_space[Slide_space_Scroll].x + (1)); //+(1) 是因为实际上计算会-1 ,这里要补回来

    } else
    {
        printf("Next_Menu:图标模式\n");
        if (Menu[Get_Menu_Id(real_Level_Id, 0)].x)
            MenuLevel[real_Level_Id].min = 1; //当前处在图标模式 如果目标层菜单的第一项为标题，则给予屏蔽
        sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1,
                        MenuLevel[real_Level_Id].x);
        *Slide_space[Slide_space_Scroll].x = 0;
    }
}
/*
    @函数 Next_Menu
    @brief 多级菜单跳转初始化参数
*/
void Next_Menu()
{
    printf("Next_Menu\n");
    //清除按键缓存
    SYSKey = NULL;
    //设置菜单标志位
    Menu_System_State = 1;

    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    uint8_t Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, 0);

    //设置编码器
    MenuSYS_SetCounter();

    if (*Switch_space[SwitchSpace_SmoothAnimation])
    {
        if (LastMenuLevelId != MenuLevelId)
        {
            Disp.setDrawColor(0);
            Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 20 * *Switch_space[SwitchSpace_SmoothAnimation]);
            Disp.setDrawColor(1);
        }


        //项目归位动画
        Menu_Smooth_Animation[3].last = 0;
        Menu_Smooth_Animation[3].val = 1;
    }


    printf("退出Next_Menu \n");
}

/*
    @函数 Pop_Windows
    @brief 自适应文本大小信息弹窗
    @param (char* s):字符串首地址
    
*/
void Pop_Windows(char* s)
{
    // Disp.setCursor(0, 0);
    // Disp.print(s);
    // Display();
    //Set_Font_Size(2);
    int w = Get_UTF8_Ascii_Pix_Len(1, s) + 2;
    int h = 12;
    // for (int i = 5;i > 0;i--) {
    //     //Set_Font_Size(i);
    //     w = CNSize * Get_Max_Line_Len(s) * Get_Font_Size() / 2;
    //     //h = CNSize * Get_Font_Size() * Get_Str_Next_Line_O(s);
    //     if (w < SCREEN_COLUMN && h < SCREEN_ROW) break;
    // }
    int x = (SCREEN_COLUMN - w) / 2;
    int y = (SCREEN_ROW - h) / 2;

    Disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3,
         66 * *Switch_space[SwitchSpace_SmoothAnimation]); //<=15FPS以便人眼察觉细节变化

    int ix = 0;
    for (int i = 1; i <= 10; i++)
    {
        //震荡动画
        if (*Switch_space[SwitchSpace_SmoothAnimation]) ix = (10 * cos((i * 3.14) / 2.0)) / i;

        Disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 0);
        Disp.drawFrame(x - 1 + ix, y - 3, w + 1, h + 3);
        Disp.setDrawColor(1);
        Disp.drawRBox(x + ix, y - 2, w, h + 2, 2);
        Disp.setDrawColor(0);
        Draw_Utf(x + 1 + ix, y - 1, s);
        Disp.setDrawColor(1);
        Display();
        delay(20 * *Switch_space[SwitchSpace_SmoothAnimation]);
    }
    //Set_Font_Size(1);
}

/*
    @函数 Run_Menu_Id
    @brief 按照菜单项预设的参数执行命令
    @param uint8_t lid 菜单层对象id 
    @param uint8_t id菜单对象id
*/
void Run_Menu_Id(uint8_t lid, uint8_t id)
{
    printf("运行菜单控件\n");
    // Serial.print("菜单系统:");
    // Serial.print(lid);
    // Serial.print(" + ");
    // Serial.println(id);
    uint8_t Id;
    uint8_t real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Id = Get_Menu_Id(lid, id);
    switch (Menu[Id].x)
    {
        case 0:
        case 2:
            LastMenuLevelId = MenuLevelId; //决定是否播放转场动画
            MenuLevelId = Menu[Id].a;
            if (!MenuLevel[Menu[Id].a].a || MenuListMode)
            { //如果当前菜单层没有开启了图表化显示则对子菜单选项定向跳转执行配置

                uint8_t ExcellentLimit = (MenuLevel[MenuLevelId].max + 1) -
                                         SCREEN_FONT_ROW; //(MenuLevel[MenuLevelId].max + 1)是为了从1开始计算
                uint8_t ExcellentMedian = (SCREEN_FONT_ROW / 2); //注意：这里从1开始计数
                //计算最优显示区域
                if (Menu[Id].b == 0)
                {
                    //头只有最差显示区域
                    MenuLevel[Menu[Id].a].x = 0;
                    *Slide_space[Slide_space_Scroll].x = 0;
                } else if (Menu[Id].b > 0 && Menu[Id].b <= MenuLevel[MenuLevelId].max - ExcellentMedian)
                {
                    //中部拥有绝佳的显示区域
                    MenuLevel[Menu[Id].a].x = Menu[Id].b - 1;
                    *Slide_space[Slide_space_Scroll].x = 1;
                } else
                {
                    //靠后位置 以及 最差的尾部
                    MenuLevel[Menu[Id].a].x = ExcellentLimit;
                    *Slide_space[Slide_space_Scroll].x = Menu[Id].b - ExcellentLimit;
                }
                // printf("MenuLevelId:%d\nMenuLevel[MenuLevelId].x:%d\n*Slide_space[Slide_space_Scroll].x:%d\n", MenuLevelId, MenuLevel[MenuLevelId].x, *Slide_space[Slide_space_Scroll].x);
            } else
            {
                //当前是图标模式
                MenuLevel[Menu[Id].a].x = Menu[Id].b;
            }
            //按需求跳转完成后执行函数
            if (Menu[Id].function) Menu[Id].function();
            //检查“跳转即退出”标志
            if (Menu_JumpAndExit && MenuLevelId == Menu_JumpAndExit_Level) Save_Exit_Menu_System();
            //再次确认菜单状态
            if (Menu_System_State) Next_Menu();         //由于执行函数可能会导致菜单状态被更改，所以这里需要确定菜单状态
            break;
        case 1:
            //Pop_Windows("正在处理");
            if (Menu[Id].function) Menu[Id].function();
            MenuSYS_SetCounter();
            break;
        case 3:
            *Switch_space[Menu[Id].a] = !*Switch_space[Menu[Id].a];
            if (Menu[Id].function) Menu[Id].function();
            break;
        case 4:
            //*Switch_space[Menu[Id].a]=!*Switch_space[Menu[Id].a];
            sys_Counter_Set(Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max,
                            Slide_space[Menu[Id].a].step, *Slide_space[Menu[Id].a].x);
            Disp.setDrawColor(0);
            Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 11 * *Switch_space[SwitchSpace_SmoothAnimation]);
            Disp.setDrawColor(1);

            while (!sys_KeyProcess())
            {
                Disp.setDrawColor(0);
                Disp.drawBox(SCREEN_COLUMN / 8 - 2, (SCREEN_ROW - 24) / 2 - 3, 3 * SCREEN_COLUMN / 4 + 4,
                             24 + 4);
                Disp.setDrawColor(1);

                Disp.drawRFrame(SCREEN_COLUMN / 8 - 3, (SCREEN_ROW - 24) / 2 - 4, 3 * SCREEN_COLUMN / 4 + 4,
                                24 + 6, 2);

                *Slide_space[Menu[Id].a].x = sys_Counter_Get();
                Draw_Utf(SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2, Menu[Id].name);
                Draw_Num_Bar(*Slide_space[Menu[Id].a].x, Slide_space[Menu[Id].a].min,
                             Slide_space[Menu[Id].a].max, SCREEN_COLUMN / 8,
                             (SCREEN_ROW - 24) / 2 + CNSize + 3, 3 * SCREEN_COLUMN / 4, 7, 1);

                Display();
                //当前滑动条为屏幕亮度调节 需要特殊设置对屏幕亮度进行实时预览
                if (Menu[Id].function) Menu[Id].function();
            }
            delay(50);

            //静音
            //PlayTones(0, 0, 0);

            //sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
            if (Menu[Id].function) Menu[Id].function();
            MenuSYS_SetCounter();
            break;

        case 5://单选模式
            *Switch_space[Menu[Id].a] = Menu[Id].b;
            if (Menu[Id].function) Menu[Id].function();
            break;
    }
}

void Draw_APP(int x, int y, uint8_t* bitmap)
{
    Disp.setDrawColor(1);
    Disp.drawRBox(x - 3, y - 3, 42 + 6, 42 + 6, 4);
    Disp.setDrawColor(0);

    Disp.setDrawColor(1);
    Draw_Slow_Bitmap_Resize(x, y, bitmap + 1, bitmap[0], bitmap[0], 42, 42);
}


static int Menu_Smooth_Animation_Y = 0;
static int Menu_Smooth_Animation_Last_choose = 0;
/*
    @函数 Menu_Control
    @brief 渲染主菜单
    @param -
*/
void Menu_Control()
{
    //printf("MenuLevelId:%d\nMenuLevel[MenuLevelId].x:%d\n*Slide_space[Slide_space_Scroll].x:%d\n", MenuLevelId, MenuLevel[MenuLevelId].x, *Slide_space[Slide_space_Scroll].x);
    if (!Menu_System_State) return;
    Disp.clearBuffer();

    //计算过渡动画
    if (*Switch_space[SwitchSpace_SmoothAnimation]) Smooth_Animation_System();

    //分别获取 菜单层、菜单项 索引值
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id,
                         MenuLevel[real_Level_Id].x + (int) *Slide_space[Slide_space_Scroll].x);

    //若当前菜单层级没有开题图标化则使用普通文本菜单的模式进行渲染显示 若屏幕分辨率低于128*32 则强制启用文本菜单模式
    if (!MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].a || MenuListMode || SCREEN_ROW <= 32)
    {

        //分别获取 菜单层、菜单项 索引值
        //int id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x);
        //int Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x);
        //显示菜单项目名::这里有两行文字是在屏幕外 用于动过渡动画
        for (int i = -1; i < SCREEN_PAGE_NUM / 2 + 1; i++)
        {
            if (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i >= 0 &&
                MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i <= MenuLevel[real_Level_Id].max)
            {

                //绘制目录树
                if (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2)
                {
                    //Set_Font_Size(2);
                    Disp.drawUTF8(0, (1 - Menu_Smooth_Animation[3].x * (i != -1)) *
                                     ((i + Menu_Smooth_Animation[0].x) * 16 + 1),
                                  Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x == 0
                                  ? "+" : "-");
                }
                //绘制目录名
                Draw_Utf(7 * (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2),
                         (1 - Menu_Smooth_Animation[3].x * (i != -1)) *
                         ((i + Menu_Smooth_Animation[0].x) * 16 + 1),
                         Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].name);

                //对特殊菜单控件的分类渲染
                switch (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x)
                {

                    //开关控件
                    case 3:
                        Draw_Utf(SCREEN_COLUMN - 32 - 1, (i + Menu_Smooth_Animation[0].x) * 16 + 1,
                                 *Switch_space[Menu[Get_Menu_Id(real_Level_Id,
                                                                MenuLevel[real_Level_Id].x + i)].a]
                                 ? (char*) "开启" : (char*) "关闭");
                        break;

                        //滑动条
                    case 4:
                        char buffer[20];
                        sprintf(buffer, "%.2f", *Slide_space[Menu[Get_Menu_Id(real_Level_Id,
                                                                              MenuLevel[real_Level_Id].x +
                                                                              i)].a].x);
                        Draw_Utf(SCREEN_COLUMN - 9 - Disp.getUTF8Width(buffer), \
                        (int) ((i + Menu_Smooth_Animation[0].x) * 16), \
                        buffer);
                        break;

                        //单选框
                    case 5:
                        if ((*Switch_space[Menu[Get_Menu_Id(real_Level_Id,
                                                            MenuLevel[real_Level_Id].x + i)].a] ==
                             Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].b))
                        {
                            Draw_Slow_Bitmap(SCREEN_COLUMN - 32 - 1 + 15, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            CheckBoxSelection, \
                            10, 10);
                        } else
                        {
                            Disp.drawFrame(SCREEN_COLUMN - 32 - 1 + 15, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            10, 10);
                        }
                        //当前项高亮
                        if ((int) *Slide_space[Slide_space_Scroll].x == i)
                        {
                            Disp.setDrawColor(2);
                            Disp.drawBox(SCREEN_COLUMN - 32 - 2 + 15, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 1, \
                            12, 12);
                            Disp.setDrawColor(1);
                        }

                        break;
                    default:
                        break;
                }

            }
        }

        //绘制滚动条
        Draw_Scale(SCREEN_COLUMN - RollingStripWidth, 0, RollingStripWidth, SCREEN_ROW - 1,
                   MenuLevel[real_Level_Id].max + 1,
                   map(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x,
                       0, MenuLevel[real_Level_Id].max + 1,
                       -Menu_Smooth_Animation[1].x * (SCREEN_ROW / (MenuLevel[real_Level_Id].max + 1)),
                       SCREEN_ROW - 1));

        //显示页码角标
        Page_Footnotes(MenuLevel[real_Level_Id].x + 1 + (int) *Slide_space[Slide_space_Scroll].x,
                       MenuLevel[real_Level_Id].max + 1);

        //反色高亮被选项
        Disp.setDrawColor(2);
        Disp.drawRBox(0, \
                            ((int) *Slide_space[Slide_space_Scroll].x - Menu_Smooth_Animation[1].x) * 16, \
*Switch_space[SwitchSpace_OptionStripFixedLength] ? 123 : (Get_UTF8_Ascii_Pix_Len(1, Menu[Pos_Id].name) -
                                                           Menu_Smooth_Animation[2].x +
                                                           12 * (Menu[Pos_Id].x != 2) + 1), \
                            CNSize + 2, \
                            0);
        Disp.setDrawColor(1);

        //项目滚动处理
        *Slide_space[Slide_space_Scroll].x = sys_Counter_Get() - 1;
        if ((int) *Slide_space[Slide_space_Scroll].x >= Slide_space[Slide_space_Scroll].max)
        {
            Log(LOG_INFO, "尝试往下滚动");
            MenuLevel[real_Level_Id].x++;
            sys_Counter_SetVal(Slide_space[Slide_space_Scroll].max);
        } else if ((int) *Slide_space[Slide_space_Scroll].x <= -1)
        {
            Log(LOG_INFO, "尝试往上滚动");
            MenuLevel[real_Level_Id].x--;
            sys_Counter_SetVal(1);
        }
        //编码器控制页内选择框滚动选择
        //CountMax = constrain(MenuLevel[real_Level_Id].max - MenuLevel[real_Level_Id].x + 1, 0, 7);
        *Slide_space[Slide_space_Scroll].x = constrain((int) *Slide_space[Slide_space_Scroll].x, 0,
                                                       Slide_space[Slide_space_Scroll].max - 1);

        // *Slide_space[Slide_space_Scroll].x = constrain(*Slide_space[Slide_space_Scroll].x, 0, min((int)Slide_space[Slide_space_Scroll].max - 2, (int)MenuLevel[real_Level_Id].max));
        MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x = constrain(
            MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x, \
MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].min, \
(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max > Slide_space[Slide_space_Scroll].max - 1) ? (
            MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max - (Slide_space[Slide_space_Scroll].max - 1))
                                                                                               : 0);

        //更新过渡动画
        real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
        Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,
                             MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x +
                             (int) *Slide_space[Slide_space_Scroll].x);
        Menu_Smooth_Animation[0].val = MenuLevel[real_Level_Id].x;
        Menu_Smooth_Animation[1].val = MenuLevel[real_Level_Id].x + (int) *Slide_space[Slide_space_Scroll].x;
        Menu_Smooth_Animation[2].val = Get_UTF8_Ascii_Pix_Len(1, Menu[Pos_Id].name);

    } else
    {
        /*  当前菜单使用了图标化的渲染方式 该模式仅支持128*64的屏幕 若宏定义中选择了128*32的屏幕将自动切换为普通文本模式显示菜单
            接受两种尺寸的图标 14x14(推荐) 和 48*48 （不推荐）
            如果为14x14在128*64屏幕中会自动放大到48*48
        */
        int id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x);
        int Pos_Id;

        //居中显示项目名
        Draw_Utf(UTF8_HMiddle(0, 128, 1, Menu[id].name), 50, Menu[id].name);


        for (uint8_t i = 0; i < 5; i++)
        {
            Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + i - 2);

            if (MenuLevel[real_Level_Id].x - 2 + i >= 0 &&
                MenuLevel[real_Level_Id].x - 2 + i <= MenuLevel[real_Level_Id].max)
            {
                //绘制菜单项目图标
                if (Menu[id].x != 2)
                {
                    if (Menu[Pos_Id].x != 2)
                    {
                        Draw_APP((1 - Menu_Smooth_Animation[3].x * (i != -1)) *
                                 (-69 + i * 56 + Menu_Smooth_Animation[0].x * 56), 3, Menu[Pos_Id].icon);
                    }
                }
            }
        }

        // Serial.print("x1:");
        // Serial.print(MenuLevel[real_Level_Id].x);

        MenuLevel[real_Level_Id].x = sys_Counter_Get();
        Menu_Smooth_Animation[0].val = MenuLevel[real_Level_Id].x;

        // Serial.print(" x2:");
        // Serial.print(MenuLevel[real_Level_Id].x);

        // Serial.print(" 编码器:");
        // Serial.println(sys_Counter_Get());

    }
    //编码器按下事件
    //菜单被选项激活 触发菜单被选项预设事件
    switch (SYSKey)
    {
        case 1:
        case 3:
            //单击和双击则执行当前项目
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,
                        MenuLevel[real_Level_Id].x + *Slide_space[Slide_space_Scroll].x);
            break;
        case 2:
            //长按执行 标题跳转
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, 0);
            break;

        default:
            break;
    }

    Display();
}
