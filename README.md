# stm32-button-led-control

基于 STM32F103C8T6 的 GPIO 入门练习项目。

本项目主要实现 LED、按键、蜂鸣器和光敏电阻模块的基础控制，用于练习 STM32 标准库工程结构、GPIO 输入输出、模块化编程以及简单的外设联动逻辑。

当前分支为：

```text
v3.0-ldr-led-button-control
```

该分支在原有按键控制 LED 的基础上，新增了光敏电阻模块，并在 `main.c` 中加入光敏模块状态与按键状态共同控制 LED 的逻辑。

---

## 功能说明

当前项目已实现以下功能：

- 使用 GPIO 控制 LED
- 使用 GPIO 读取独立按键状态
- 支持两个按键独立检测
- 使用按键状态控制不同 LED 翻转
- 新增光敏电阻模块
- 通过 `Light_Sensor_GetState()` 读取光敏模块状态
- 光敏模块触发时参与 LED 控制
- 保留蜂鸣器模块作为扩展外设

---

## 当前主程序逻辑

`main.c` 中包含以下模块头文件：

```c
#include "stm32f10x.h"
#include "LED.h"
#include "buzzer.h"
#include "KEY.h"
#include "Light_Sensor.h"
```

程序初始化部分：

```c
LED_Init();
Key_Init();
Light_Sensor_Init();
```

主循环逻辑：

```c
while(1)
{
    KeyNum = Key_GetNum();

    if(KeyNum & 0x01 || Light_Sensor_GetState() == 1)
    {
        LEDA0_TURN();
    }

    if(KeyNum & 0x02)
    {
        LEDC14_TURN();
    }
}
```

逻辑说明：

```text
按键 1 按下，或者光敏电阻模块状态为 1 时，翻转 PA0 对应 LED。

按键 2 按下时，翻转 PC14 对应 LED。
```

---

## 按键返回值说明

`Key_GetNum()` 函数使用位标志返回按键状态：

```text
0x01：表示按键 1 被触发
0x02：表示按键 2 被触发
```

因此可以通过按位与判断某一个按键是否被触发：

```c
if(KeyNum & 0x01)
{
    // 按键 1 被触发
}

if(KeyNum & 0x02)
{
    // 按键 2 被触发
}
```

这种写法的好处是：  
多个按键的状态可以同时记录在同一个变量中，便于后续扩展更多按键。

---

## 光敏电阻模块说明

本分支新增了光敏电阻模块文件：

```text
Light_Sensor.c
Light_Sensor.h
```

光敏电阻模块通过 `Light_Sensor_GetState()` 函数返回当前光照检测状态。

当前主程序中使用：

```c
Light_Sensor_GetState() == 1
```

来判断光敏模块是否触发。

注意：  
不同光敏电阻模块的输出逻辑可能不同，有些模块在光照强时输出高电平，有些模块在光照弱时输出高电平。实际效果需要根据模块接线和 `Light_Sensor.c` 中的判断逻辑来确认。

---

## 项目结构

```text
stm32-button-led-control
├── Hardware
│   ├── LED.c
│   ├── LED.h
│   ├── KEY.c
│   ├── KEY.h
│   ├── BUZZER.c
│   ├── BUZZER.h
│   ├── Light_Sensor.c
│   └── Light_Sensor.h
├── Library
├── Start
├── System
├── User
│   ├── main.c
│   ├── stm32f10x_conf.h
│   ├── stm32f10x_it.c
│   └── stm32f10x_it.h
├── GPIOOUT.uvprojx
├── GPIOOUT.uvoptx
└── README.md
```

---

## 文件说明

### `User/main.c`

主程序入口文件。

主要完成 LED、按键和光敏电阻模块初始化，并在 `while(1)` 主循环中不断读取按键与光敏模块状态，根据条件控制 LED 翻转。

---

### `Hardware/LED.c` / `Hardware/LED.h`

LED 控制模块。

主要封装 LED 初始化、点亮、熄灭、翻转等操作。

当前主程序中使用的 LED 控制函数包括：

```c
LEDA0_TURN();
LEDC14_TURN();
```

---

### `Hardware/KEY.c` / `Hardware/KEY.h`

按键检测模块。

当前用于检测两个独立按键，并通过 `Key_GetNum()` 返回按键状态。

当前按键逻辑中使用位标志记录按键触发状态：

```text
0x01：按键 1
0x02：按键 2
```

---

### `Hardware/Light_Sensor.c` / `Hardware/Light_Sensor.h`

光敏电阻模块。

主要用于初始化光敏电阻检测引脚，并读取光敏模块输出状态。

当前主程序通过以下函数读取光敏模块状态：

```c
Light_Sensor_GetState();
```

---

### `Hardware/BUZZER.c` / `Hardware/BUZZER.h`

蜂鸣器模块。

当前作为扩展外设保留，后续可用于加入声音提示功能。

---

## 硬件模块

本项目使用的主要硬件包括：

- STM32F103C8T6 最小系统板
- LED 模块
- 独立按键模块
- 蜂鸣器模块
- 光敏电阻模块

---

## 当前控制效果

当前 `v3.0-ldr-led-button-control` 分支的主要控制效果如下：

```text
按键 1 触发
或
光敏电阻模块触发
        ↓
翻转 PA0 LED
```

```text
按键 2 触发
        ↓
翻转 PC14 LED
```

---

## 版本记录

### v1.0

完成基础按键控制 LED 功能。

主要内容：

- 初始化 LED
- 初始化按键
- 实现按键控制 LED，独立控制：长按LED亮，松手LED灭

---

### v2.0-refactor

重构按键检测逻辑。

主要内容：

- 优化按键检测代码
- 修复双按键独立检测相关问题
- 使用按键状态记录方式处理多个按键

---

### v3.0-ldr-led-button-control

新增光敏电阻模块，并修改主程序控制逻辑。
重构按键控制LED代码：实现阻塞式按键控制LED状态翻转，并进行按键消抖。

主要内容：

- 添加 `Light_Sensor.c`
- 添加 `Light_Sensor.h`
- 覆盖`KEY.c`
- 覆盖`KEY.h`
- 在 `main.c` 中初始化光敏电阻模块
- 使用 `Light_Sensor_GetState()` 读取光敏模块状态
- 实现按键状态与光敏模块状态共同参与 LED 控制

---

## 学习重点

通过本项目可以练习以下内容：

- STM32 标准库工程结构
- GPIO 输出控制
- GPIO 输入读取
- LED 控制
- 按键检测
- 按键消抖
- 多按键状态记录
- 模块化编程
- 光敏电阻模块状态读取
- 简单外设联动控制

---

## 后续计划

后续可以继续扩展以下功能：

- 优化按键消抖逻辑
- 优化实现非阻塞式消抖
- 加入蜂鸣器提示功能
- 将光敏电阻模块与蜂鸣器联动
- 后续学习 ADC 后，可尝试读取光敏电阻的模拟量变化
- 后续加入 OLED 后，可将按键状态和光敏模块状态显示出来

---

## 说明

本项目是 STM32 入门学习过程中的练习项目，代码会随着学习进度持续更新。

当前版本重点不是追求完整功能，而是通过 LED、按键和光敏电阻模块，理解 GPIO 输入输出、模块化文件管理以及简单控制逻辑。
STM32F103C8T6 按键控制 LED 示例，实现长按亮、松手灭，并通过按位|& 状态记录修复双按键检测 Bug。
