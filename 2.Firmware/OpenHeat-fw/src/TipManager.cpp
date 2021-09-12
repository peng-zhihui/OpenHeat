#include "OpenT12.h"

uint8_t TipID = 0;
uint8_t TipTotal = 1;

TIPCONFIG MyTip[MaxTipConfig] = {
    {{51.6128997803, 0.1443523616, 0.0000061750, -0.0000000001}, {{30.0, 0, 0.5}, {20.0, 0, 0.5}}, "智能加热台"},
};

/*** 
 * @description: 载入烙铁头配置到系统环境
 * @param {*}
 * @return {*}
 */
void LoadTipConfig(void)
{
    //载入温度系数
    for (uint8_t i = 0; i < 4; i++)
    {
        PTemp[i] = MyTip[TipID].PTemp[i];
    }
    //载入PID参数
    aggKp = MyTip[TipID].PID[0][0];
    aggKi = MyTip[TipID].PID[0][1];
    aggKd = MyTip[TipID].PID[0][2];

    consKp = MyTip[TipID].PID[1][0];
    consKi = MyTip[TipID].PID[1][1];
    consKd = MyTip[TipID].PID[1][2];
    //载入名称
    TipName = MyTip[TipID].name;

}

/***
 * @description: 保存烙铁头配置到系统环境
 * @param {*}
 * @return {*}
 */
void SaveTipConfig(void)
{
    //载入温度系数
    for (uint8_t i = 0; i < 4; i++)
    {
        MyTip[TipID].PTemp[i] = PTemp[i];
    }
    //载入PID参数
    MyTip[TipID].PID[0][0] = aggKp;
    MyTip[TipID].PID[0][1] = aggKi;
    MyTip[TipID].PID[0][2] = aggKd;

    MyTip[TipID].PID[1][0] = consKp;
    MyTip[TipID].PID[1][1] = consKi;
    MyTip[TipID].PID[1][2] = consKd;

    //由于名称是用指针传递，所以不需要重新赋值
}

/*** 
 * @description: 更新菜单配置列表
 * @param {*}
 * @return {*}
 */
void FlashTipMenu(void)
{
    uint8_t MLID = Get_Real_Menu_Level_Id(15); //查询配置列表所在的菜单系统层级
    uint8_t MID;

    MenuLevel[MLID].max = TipTotal;  //修改菜单最大项目数量

    for (uint8_t i = 0; i < TipTotal; i++)
    {
        MID = Get_Menu_Id(MLID, i + (1));    //查询配置列表项目id,另外 +(1)是为了跳过标题
        Menu[MID].name = MyTip[i].name;    //刷新名称

    }
}
/*** 
 * @description: 新建配置
 * @param {*}
 * @return {*}
 */
void NewTipConfig(void)
{
    if (TipTotal < MaxTipConfig)
    {
        TipID = TipTotal;

        // char NewName = "---未命名---";
        strcpy(MyTip[TipTotal].name, "NewTip");

        for (uint8_t i = 0; i < 4; i++)
        {
            MyTip[TipID].PTemp[i] = 0;
        }

        MyTip[TipID].PID[0][0] = 0;
        MyTip[TipID].PID[0][1] = 0;
        MyTip[TipID].PID[0][2] = 0;
        MyTip[TipID].PID[1][0] = 0;
        MyTip[TipID].PID[1][1] = 0;
        MyTip[TipID].PID[1][2] = 0;


        TipTotal++;
        Pop_Windows("新建成功");

        TipRename();
    } else Pop_Windows("达到上限");

    //重新加载烙铁头参数
    LoadTipConfig();
}

/*** 
 * @description: 重命名当前的配置
 * @param {*}
 * @return {*}
 */
void TipRename(void)
{
    TextEditor("[重命名]", MyTip[TipID].name);
}

/*** 
 * @description: 删除配置
 * @param {*}
 * @return {*}
 */
void TipDel(void)
{
    if (TipTotal > 1)
    {
        //覆盖并移动配置
        for (uint8_t i = TipID; i < TipTotal - 1; i++)
        {
            MyTip[i] = MyTip[i + 1];
        }
        TipTotal--;
        Pop_Windows("删除成功");
    } else Pop_Windows("这是最后一个配置");

    //重新加载烙铁头参数
    LoadTipConfig();
}