#include "OpenT12.h"

static double Count = 0;
static double CountLast = 0;
static double Count_min = 0;
static double Count_max = 0;
static double Count_step = 0;
static uint8_t CounterChanged = 0;

void sys_RotaryInit(void)
{
    //初始化GPIO
    pinMode(ROTARY_PIN1, INPUT_PULLUP);
    pinMode(ROTARY_PIN2, INPUT_PULLUP);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    //初始化按键事件检测
    RButton.attachClick(sys_Counter_click);
    RButton.attachDoubleClick(sys_Counter_doubleclick);
    RButton.attachLongPressStart(sys_Counter_longclick);
    RButton.setDebounceTicks(25);
    RButton.setClickTicks(30);
    RButton.setPressTicks(500);

    //初始化编码器中断
    attachInterrupt(ROTARY_PIN1, sys_Counter_IRQHandler, CHANGE);

    //初始化编码器设置(测试默认)
    sys_Counter_Set(-1.2, 6.6, 0.1, 3.14);
}

/**
 * @name  sys_Counter_Set
 * @brief 设置计数器参数
 * @param {double} c       计数器初始值
 * @param {double} min     计数器最小值
 * @param {double} max     计数器最大值
 * @param {double} step    计数器增量步进
 * @return {*}
 */
void sys_Counter_Set(double min, double max, double step, double c)
{
    delay(100);
    Count_min = min * ROTARY_TYPE;
    Count_max = max * ROTARY_TYPE;
    Count_step = step;
    Count = constrain(c * ROTARY_TYPE, Count_min, Count_max);
    Serial.print("设置编码器 -> 最小值:");
    Serial.print(Count_min);
    Serial.print(" 最大值:");
    Serial.print(Count_max);
    Serial.print(" 步进:");
    Serial.print(Count_step);
    Serial.print(" 传入计数:");
    Serial.print(c);
    Serial.print(" 当前计数:");
    Serial.println(Count);
}

void sys_Counter_SetVal(double c)
{
    Count = constrain(c * ROTARY_TYPE, Count_min, Count_max);
}

void RotaryUp(void)
{
    sys_Counter_SetVal(sys_Counter_Get() + Count_step);
}

void RotaryDown(void)
{
    sys_Counter_SetVal(sys_Counter_Get() - Count_step);
}

bool Counter_LOCK_Flag = false;
void ICACHE_RAM_ATTR sys_Counter_IRQHandler(void)
{
    //重置事件计时器
    TimerUpdateEvent();

    //若编码器被锁定，则不允许数值操作
    if (Counter_LOCK_Flag == true) return;

    //更新编码器方向
    double step = (RotaryDirection == 0) ? Count_step : -Count_step;

    static volatile uint8_t a0, b0;
    static volatile uint8_t ab0;
    uint8_t a = digitalRead(ROTARY_PIN1);
    uint8_t b = digitalRead(ROTARY_PIN2);
    if (a != a0)
    {
        a0 = a;
        if (b != b0)
        {
            b0 = b;
            Count = constrain(Count + ((a == b) ? step : -step), Count_min, Count_max);
            if ((a == b) != ab0)
            {
                Count = constrain(Count + ((a == b) ? step : -step), Count_min, Count_max);
            }
            ab0 = (a == b);
        }
    }
    //printf("编码器:%lf\n", sys_Counter_Get());

    if (Count != CountLast)
    {
        CountLast = Count;
        CounterChanged = 1;
    }

}

double sys_Counter_Get(void)
{
    // if (Count != CountLast) {
    //     CountLast = Count;
    //     printf("C:%lf\n",Count);
    // }else printf("C:nc\n", Count);
    return Count / ROTARY_TYPE;
}

uint8_t sys_Counter_Change(void)
{
    uint8_t flag = CounterChanged;
    CounterChanged = 0;
    return flag;
}

//按键FIFO循环大小
#define RButton_FIFO_Size 10
//按键FIFO读写指针
static uint8_t RButton_FIFO_pwrite = 0;
static uint8_t RButton_FIFO_pread = 0;
//按键FIFO缓存区
static uint8_t RButton_FIFO[RButton_FIFO_Size];
//按键FIFO缓存区有效数据大小
static uint8_t RButton_FIFO_BufferSize = 0;


void Clear_RButton_FIFO(void)
{
    RButton_FIFO_pread = RButton_FIFO_pwrite;
    memset(RButton_FIFO, 0, RButton_FIFO_Size);
}
/*** 
 * @description: 写按键FIFO
 * @param uint8_t State 要写入FIFO的按键状态值
 * @return 无
 */
//
static void Write_RButton_FIFO(uint8_t State)
{
    //重置事件计时器
    TimerUpdateEvent();

    RButton_FIFO[RButton_FIFO_pwrite] = State;
    printf("FIFO写入[%d]=%d\n", RButton_FIFO_pwrite, State);
    //写指针移位
    RButton_FIFO_pwrite++;
    //按键缓冲区数据大小+1
    if (RButton_FIFO_BufferSize < RButton_FIFO_Size) RButton_FIFO_BufferSize++;
    //循环写
    if (RButton_FIFO_pwrite >= RButton_FIFO_Size) RButton_FIFO_pwrite = 0;
    printf("FIFO缓存区大小:%d\n\n", RButton_FIFO_BufferSize);
}
/*** 
 * @description: 读按键FIFO
 * @param {*}
 * @return 按键State
 */
//读按键FIFO
static uint8_t Read_RButton_FIFO(void)
{
    //判断当前按键缓冲区是否有数据
    if (RButton_FIFO_BufferSize == 0) return 0;

    //从按键FIFO缓存读取数据
    uint8_t res = RButton_FIFO[RButton_FIFO_pread];
    printf("FIFO读取[%d]=%d\n", RButton_FIFO_pread, res);
    //读指针移位
    RButton_FIFO_pread++;
    RButton_FIFO_BufferSize--;
    //循环写
    if (RButton_FIFO_pread >= RButton_FIFO_Size) RButton_FIFO_pread = 0;
    printf("FIFO缓存区大小:%d\n\n", RButton_FIFO_BufferSize);
    return res;
}
/*** 
 * @description: 按键单击回调函数
 * @param {*}
 * @return {*}
 */
void sys_Counter_click(void)
{
    printf("触发单击事件\n");
    SetSound(Beep1);
    Write_RButton_FIFO(1);
}
/*** 
 * @description: 按键长按回调函数
 * @param {*}
 * @return {*}
 */
void sys_Counter_longclick(void)
{
    printf("触发长按事件\n");
    SetSound(Beep2);
    Write_RButton_FIFO(2);
}
/*** 
 * @description: 按键双击回调函数
 * @param {*}
 * @return {*}
 */
void sys_Counter_doubleclick(void)
{
    printf("触发双击事件\n");
    SetSound(Beep2);
    Write_RButton_FIFO(3);
}

//编码器按键按下 + 软件滤波
uint8_t SYSKey = 0;
uint8_t sys_KeyProcess(void)
{
    RButton.tick();
    SYSKey = Read_RButton_FIFO();
    return SYSKey;
}