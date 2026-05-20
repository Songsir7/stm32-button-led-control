# 双按键独立检测 Bug 修复报告

## 一、问题现象记录

本次实验使用的是 **STM32F103 系列单片机**，按键连接在 `GPIOA` 端口上。

### 1. 硬件按键配置

| 引脚 | 功能 | 输入模式 | 按下状态 |
|---|---|---|---|
| PA8 | 按键输入 | 上拉输入 | 低电平表示按下 |
| PA11 | 按键输入 | 上拉输入 | 低电平表示按下 |

### 2. LED 控制关系

| 按键 | 控制 LED |
|---|---|
| PA8 | LEDA0 |
| PA11 | LEDC14 |

### 3. 预期效果

- 按住 `PA8`，`LEDA0` 亮；松开 `PA8`，`LEDA0` 灭。
- 按住 `PA11`，`LEDC14` 亮；松开 `PA11`，`LEDC14` 灭。
- 两个按键同时按下时，两个 LED 都亮。
- 松开其中任意一个按键时，只熄灭对应的 LED。

### 4. 实际异常现象

实际测试时出现了异常：

- 先按下 `PA8` 不松手，再按下 `PA11`，`LEDC14` 不会亮。
- 但是反过来，先按下 `PA11` 不松手，再按下 `PA8`，`LEDC14` 又可以保持亮。

这个现象说明程序并没有真正做到两个按键独立检测，而是存在 **按键判断优先级** 和 **状态覆盖** 的问题。

---

## 二、错误版本代码记录 v1.0

### 1. `Key.c`

```c
#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
    // PA8 和 PA11 作为按键输入，低电位驱动，上拉输入
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 不用配置高电位，因为是上拉输入，默认就是高电位
}

uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;

    // 先不考虑抖动过滤，直接读取按键状态
    // 也不加阻塞，直接返回按键状态

    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
    {
        KeyNum = 1;
    }
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
    {
        KeyNum = 2;
    }
    else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 1 |
            GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 1)
    {
        KeyNum = 0;
    }

    return KeyNum;
}
```

### 2. `main.c`

```c
#include "stm32f10x.h"                  // Device header
#include "Delay.h"                      // Delay header
#include "LED.h"                        // LED header
#include "buzzer.h"                     // Buzzer header
#include "KEY.h"                        // Key header

int main(void)
{
    LED_Init();
    Buzzer_Init();
    Key_Init();

    while(1)
    {
        // 按键 PA8
        if(Key_GetNum() == 1)
        {
            LEDA0_ON();
        }

        // 按键 PA11
        if(Key_GetNum() == 2)
        {
            LEDC14_ON();
        }

        if(Key_GetNum() == 0)
        {
            LEDA0_OFF();
            LEDC14_OFF();
        }
    }
}
```

---

## 三、Bug 逻辑问题分析

这个 Bug 的核心问题主要有三个。

### 1. `Key_GetNum()` 使用了互斥判断结构

原来的 `Key_GetNum()` 使用了：

```c
if (...)
{
    ...
}
else if (...)
{
    ...
}
else if (...)
{
    ...
}
```

在这个结构中，`PA8` 的判断写在最前面。

所以只要 `PA8` 被按下，程序就会直接返回：

```c
KeyNum = 1;
```

后面的 `PA11` 判断就不会再执行。

因此，当 `PA8` 和 `PA11` 同时按下时，虽然两个引脚都是低电平：

```c
PA8  = 0
PA11 = 0
```

但是程序只会检测到 `PA8`，返回：

```c
KeyNum = 1;
```

不会返回 `PA11` 的状态。

所以先按住 `PA8`，再按 `PA11` 时，`LEDC14` 不会亮。

---

### 2. `main.c` 中多次调用 `Key_GetNum()`

原来的主函数中多次调用了 `Key_GetNum()`：

```c
if(Key_GetNum() == 1)
if(Key_GetNum() == 2)
if(Key_GetNum() == 0)
```

这样每个判断都会重新读取一次按键状态，三个判断不是基于同一次按键结果。

更好的方式应该是：

```c
KeyNum = Key_GetNum();
```

先读取一次按键状态，然后后面的判断都使用这一次读取到的 `KeyNum`。

---

### 3. 使用了按位或 `|`，不适合用于逻辑判断

原代码中有这一句：

```c
GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 1 |
GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 1
```

这里使用的是按位或：

```c
|
```

而不是逻辑或：

```c
||
```

虽然在这个场景下大多数情况下也能运行，但在 `if` 条件判断中，更推荐使用逻辑或 `||`。

不过在本次修复中，这个判断其实可以直接去掉，因为 `KeyNum` 初始值就是 `0`。如果没有任何按键按下，就直接返回 `0`。

---

## 四、修复思路 v2.0

v2.0 的核心逻辑是：

不再让 `Key_GetNum()` 返回单一按键编号，而是让它返回一个 **按键状态值**。

使用二进制位记录不同按键的状态：

| KeyNum 位 | 对应按键 |
|---|---|
| 第 0 位 | PA8 |
| 第 1 位 | PA11 |

返回值含义如下：

| KeyNum | 二进制 | 含义 |
|---|---|---|
| 0 | 0000 0000 | 两个按键都没有按下 |
| 1 | 0000 0001 | PA8 按下 |
| 2 | 0000 0010 | PA11 按下 |
| 3 | 0000 0011 | PA8 和 PA11 同时按下 |

这样就可以做到两个按键互不影响。

在 `main.c` 中，每次循环只调用一次 `Key_GetNum()`，然后使用按位与 `&` 判断某一位是否为 `1`。

---

## 五、最新版本代码 v2.0

### 1. 最新版本 `Key.c`

```c
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "KEY.h"

void Key_Init(void)
{
    // PA8 和 PA11 作为按键输入，低电平表示按下，上拉输入
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启 GPIOA 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * 返回按键状态
 *
 * KeyNum = 0：没有按键按下
 * KeyNum = 1：PA8 按下
 * KeyNum = 2：PA11 按下
 * KeyNum = 3：PA8 和 PA11 同时按下
 */
uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;

    // PA8 按下，记录第 0 位
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
    {
        KeyNum |= 0x01;
    }

    // PA11 按下，记录第 1 位
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
    {
        KeyNum |= 0x02;
    }

    return KeyNum;
}
```

---

### 2. 最新版本 `main.c`

```c
#include "stm32f10x.h"                  // Device header
#include "Delay.h"                      // Delay header
#include "LED.h"                        // LED header
#include "buzzer.h"                     // Buzzer header
#include "KEY.h"                        // Key header

int main(void)
{
    uint8_t KeyNum;

    LED_Init();
    Buzzer_Init();
    Key_Init();

    while(1)
    {
        // 每次循环只读取一次按键状态
        KeyNum = Key_GetNum();

        // 判断 PA8 是否按下
        // PA8 按下，LEDA0 亮；PA8 松开，LEDA0 灭
        if(KeyNum & 0x01)
        {
            LEDA0_ON();
        }
        else
        {
            LEDA0_OFF();
        }

        // 判断 PA11 是否按下
        // PA11 按下，LEDC14 亮；PA11 松开，LEDC14 灭
        if(KeyNum & 0x02)
        {
            LEDC14_ON();
        }
        else
        {
            LEDC14_OFF();
        }
    }
}
```

---

## 六、修复效果

修改后，`Key_GetNum()` 不再只返回一个按键编号，而是通过按位或 `|=` 组合多个按键状态。

例如：

```c
KeyNum |= 0x01;
```

表示记录 `PA8` 被按下。

```c
KeyNum |= 0x02;
```

表示记录 `PA11` 被按下。

如果两个按键同时按下，最终结果为：

```c
KeyNum = 0x01 | 0x02;
KeyNum = 0x03;
```

也就是：

```c
KeyNum = 3;
```

主函数中再通过按位与 `&` 判断对应按键是否按下：

```c
if(KeyNum & 0x01)
```

用于判断 `PA8` 是否按下。

```c
if(KeyNum & 0x02)
```

用于判断 `PA11` 是否按下。

这样两个按键就可以独立控制对应 LED。

---

## 七、最终结论

原来的程序把多个按键当成“互斥选择”来处理，所以只能识别一个按键。

当 `PA8` 和 `PA11` 同时按下时，由于 `PA8` 的判断在前，程序会优先返回 `KeyNum = 1`，导致 `PA11` 的状态无法被继续检测。

v2.0 将按键状态改成 **按位记录**：

- `PA8` 使用 `0x01` 记录；
- `PA11` 使用 `0x02` 记录；
- 多个按键同时按下时，使用按位或组合状态；
- 主函数中使用按位与判断对应按键状态。

最终解决了两个按键不能同时独立控制 LED 的问题。
