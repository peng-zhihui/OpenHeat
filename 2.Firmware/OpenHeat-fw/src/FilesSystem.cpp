#include "OpenT12.h"

//存档路径
char SYS_SVAE_PATH[50] = "/OpenT12.sav";

void SYS_Save(void)
{
    //关闭中断
    noInterrupts();
    //Pop_Windows("保存中 请勿切断电源");

    //软盘图标
    DrawMsgBox("保存中");
    Draw_Slow_Bitmap_Resize(128 - 28 - 4, 64 - 28 - 4, Save + 1, Save[0], Save[0], 28, 28);
    Display();


    File file = SPIFFS.open(SYS_SVAE_PATH, FILE_WRITE);
    if (!file)
    {
        Log(LOG_ERROR, "存档写入失败!");
        Pop_Windows("写入失败");
        return;
    }

    //注意这个是判断版本的依据，载入不同版本的存档可能会导致致命错误
    file.write((uint8_t*) &ChipMAC, sizeof(ChipMAC));
    file.write((uint8_t*) &CompileTime, sizeof(CompileTime));

    file.write((uint8_t*) &BootTemp, sizeof(BootTemp));
    file.write((uint8_t*) &SleepTemp, sizeof(SleepTemp));
    file.write((uint8_t*) &BoostTemp, sizeof(BoostTemp));

    file.write((uint8_t*) &ShutdownTime, sizeof(ShutdownTime));
    file.write((uint8_t*) &SleepTime, sizeof(SleepTime));
    file.write((uint8_t*) &ScreenProtectorTime, sizeof(ScreenProtectorTime));
    file.write((uint8_t*) &BoostTime, sizeof(BoostTime));

    file.write((uint8_t*) &PIDMode, sizeof(PIDMode));
    file.write((uint8_t*) &Use_KFP, sizeof(Use_KFP));
    file.write((uint8_t*) &PanelSettings, sizeof(PanelSettings));
    file.write((uint8_t*) &ScreenFlip, sizeof(ScreenFlip));
    file.write((uint8_t*) &SmoothAnimation_Flag, sizeof(SmoothAnimation_Flag));
    file.write((uint8_t*) &ScreenBrightness, sizeof(ScreenBrightness));
    file.write((uint8_t*) &OptionStripFixedLength_Flag, sizeof(OptionStripFixedLength_Flag));

    file.write((uint8_t*) &KFP_Temp, sizeof(KFP_Temp));
    file.write((uint8_t*) &ADC_PID_Cycle_List, sizeof(ADC_PID_Cycle_List));
    file.write((uint8_t*) &SamplingRatioWork, sizeof(SamplingRatioWork));

    file.write((uint8_t*) &Volume, sizeof(Volume));
    file.write((uint8_t*) &RotaryDirection, sizeof(RotaryDirection));
    file.write((uint8_t*) &HandleTrigger, sizeof(HandleTrigger));

    file.write((uint8_t*) &SYS_Voltage, sizeof(SYS_Voltage));
    file.write((uint8_t*) &UndervoltageAlert, sizeof(UndervoltageAlert));
    file.write((uint8_t*) &BootPasswd, sizeof(BootPasswd));
    file.write((uint8_t*) &Language, sizeof(Language));

    file.write((uint8_t*) &MyTip, sizeof(MyTip));
    file.write((uint8_t*) &TipID, sizeof(TipID));
    file.write((uint8_t*) &TipTotal, sizeof(TipTotal));

    file.write((uint8_t*) &BLE_State, sizeof(BLE_State));
    file.write((uint8_t*) &BLE_name, sizeof(BLE_name));

    file.write((uint8_t*) &MenuListMode, sizeof(MenuListMode));

    file.close();
    Log(LOG_OK, "存档保存成功!");

    //启用中断
    interrupts();
}

void SYS_Load(void)
{
    //关闭中断
    noInterrupts();

    //软盘图标
    DrawMsgBox("加载中");
    Draw_Slow_Bitmap(128 - 28 - 4, 64 - 28 - 4, IMG_Load, 28, 28);
    Display();

    File file = SPIFFS.open(SYS_SVAE_PATH);
    if (!file.available())
    {
        Log(LOG_ERROR, "存档不存在！");
        Pop_Windows("存档不存在！");
        file.close();
        return;
    }

    //写入系统信息，载入不同版本的存档可能会导致致命错误
    uint64_t FSChipMAC;
    char FSCompileTime[20];

    file.read((uint8_t*) &FSChipMAC, sizeof(FSChipMAC));
    file.read((uint8_t*) &FSCompileTime, sizeof(FSCompileTime));

    //判断存档版本，载入不同版本的存档可能会导致致命错误
    if (memcmp(CompileTime, FSCompileTime, sizeof(FSCompileTime)))
    {
        Log(LOG_ERROR, "存档版本不一致，拒绝加载存档！");
        Pop_Windows("存档版本不一致");
        return;
    }

    file.read((uint8_t*) &BootTemp, sizeof(BootTemp));
    file.read((uint8_t*) &SleepTemp, sizeof(SleepTemp));
    file.read((uint8_t*) &BoostTemp, sizeof(BoostTemp));

    file.read((uint8_t*) &ShutdownTime, sizeof(ShutdownTime));
    file.read((uint8_t*) &SleepTime, sizeof(SleepTime));
    file.read((uint8_t*) &ScreenProtectorTime, sizeof(ScreenProtectorTime));
    file.read((uint8_t*) &BoostTime, sizeof(BoostTime));

    file.read((uint8_t*) &PIDMode, sizeof(PIDMode));
    file.read((uint8_t*) &Use_KFP, sizeof(Use_KFP));
    file.read((uint8_t*) &PanelSettings, sizeof(PanelSettings));
    file.read((uint8_t*) &ScreenFlip, sizeof(ScreenFlip));
    file.read((uint8_t*) &SmoothAnimation_Flag, sizeof(SmoothAnimation_Flag));
    file.read((uint8_t*) &ScreenBrightness, sizeof(ScreenBrightness));
    file.read((uint8_t*) &OptionStripFixedLength_Flag, sizeof(OptionStripFixedLength_Flag));

    file.read((uint8_t*) &KFP_Temp, sizeof(KFP_Temp));
    file.read((uint8_t*) &ADC_PID_Cycle_List, sizeof(ADC_PID_Cycle_List));
    file.read((uint8_t*) &SamplingRatioWork, sizeof(SamplingRatioWork));

    file.read((uint8_t*) &Volume, sizeof(Volume));
    file.read((uint8_t*) &RotaryDirection, sizeof(RotaryDirection));
    file.read((uint8_t*) &HandleTrigger, sizeof(HandleTrigger));

    file.read((uint8_t*) &SYS_Voltage, sizeof(SYS_Voltage));
    file.read((uint8_t*) &UndervoltageAlert, sizeof(UndervoltageAlert));
    file.read((uint8_t*) &BootPasswd, sizeof(BootPasswd));
    file.read((uint8_t*) &Language, sizeof(Language));

    file.read((uint8_t*) &MyTip, sizeof(MyTip));
    file.read((uint8_t*) &TipID, sizeof(TipID));
    file.read((uint8_t*) &TipTotal, sizeof(TipTotal));

    file.read((uint8_t*) &BLE_State, sizeof(BLE_State));
    file.read((uint8_t*) &BLE_name, sizeof(BLE_name));

    file.read((uint8_t*) &MenuListMode, sizeof(MenuListMode));

    file.close();

    Update_OLED_Flip();
    Update_OLED_Light_Level();
    PID_Setpoint = BootTemp;

    Log(LOG_OK, "存档读取成功!");

    //启用中断
    interrupts();
}

void FilesSystemInit(void)
{
    //关闭中断
    noInterrupts();

    if (!SPIFFS.begin(false))
    {
        Log(LOG_ERROR, "文件系统打开失败，自动格式化\n");
        Log(LOG_INFO, "格式化...");
        SPIFFS.format();
        Log(LOG_OK, "文件系统初始化完成！");
    }
    //启用中断
    interrupts();

    printf("[OK]文件系统\n");

    SYS_Load();

    // listDir(SPIFFS, "/", 0);
    // writeFile(SPIFFS, "/hello.txt", "Hello \n");
    // appendFile(SPIFFS, "/hello.txt", "World!\r\n\n");
    // readFile(SPIFFS, "/hello.txt\n");
    // renameFile(SPIFFS, "/hello.txt", "/foo.txt\n");
    // readFile(SPIFFS, "/foo.txt\n");
    // deleteFile(SPIFFS, "/foo.txt\n");
    // testFileIO(SPIFFS, "/test.txt\n");
    // deleteFile(SPIFFS, "/test.txt\n");
    // printf("Test complete\n");
}