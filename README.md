# pipe_rotbot_MotionControll
## 2018年1月15日更
 
在昨天的基础上，进行一些完善：
  1. 解决昨天的问题2，目前正在修改当中，因为中断中case 1 2 3 4的判断问题，导致当前逻辑需要进一步修正。已经可以完成前后、正负位置的精确行走
  2. 去除多余的调试代码，只通过CAN返回必须的数值。现在的功能如下：
      - 位置控制运动停止后，自动返回里程数
      - 点击停止后，自动显示里程数。
  3. 修改了数据结构，因为机器人齿轮变化后，一个cycle对应的脉冲数已经远远超出158。所以现在不得不用两个字节来表示新的数据。现在单片机上已经修改，接下来需要 **测量新的脉冲重载值**




## 2018年1月15日更
 
在昨天的基础上:
   1. 撑开与收缩停止逻辑进行了修改，即当撑开或者收缩的速度变小，这个时候认为收到了外力，已经接触到了极限值，停下来。**为了排除一些干扰因素，这里使用了平均滤波进行双层的判断，保证撑开或收缩到位，同时不至于对电机有过大的损伤；
   2. 前进后退的位置控制。可以通过上位机给定具体位置，比如20cm前进   15cm后退。 
   
存在的问题:
   1. 撑开与收缩虽然滤波后效果明显，但是不同场景下，阈值仍然会发生改变。需要重新评估
   2. 前进、后退的判断不太智能，因为只能实现比上次位置绝对值更大的运动，比如当前位置为10cm，前进15可以，但是前进到5cm处，就没有办法。
   3. **由于硬件安装问题，目前向前表示为负值，向后为正值，这个在最后完善阶段会取相反数显示**


-----------------------------------------------------
## 控制逻辑
### 运动控制
1. 运动控制是依靠电机的正反转控制方向，通过PWM信号控制转速
2. 控制协议：xx | xx | xx 3个字节
  1. 1字节 前后运动  or   撑开与收缩
  2. 2字节 前后运动: 前  or  后     扩张收缩: 扩张 or  收缩
  3. 3字节 速度控制
3. 最新的运动控制，增加了一个伺服位置控制。上位机给出具体的位置，下位机自动运动到相应位置。这里需要主要涉及到对编码器的识别与控制。在的二部分进行位置里程计的说明。

### 里程计
这里的里程计主要通过编码器的计数，通过脉冲数以及轮子一圈所走过的路程进行计算。根据设计和结构，可知轮子转一圈大概走11.3cm ，而一个脉冲所对应的距离大致为0.071cm。因此，距离有一下公式：
$$ Distance = ((cycle-127) *11.7) + (inpule * 0.071)) $$ 
#### 编码器计数与逻辑
根据实验，电机转一圈，编码器发出158个脉冲。通过软件方式增加分辨率，采取边沿检测的方式，即计数器的重载值为 $158 * 4$。
在中断中，设置一个cycle值，来表示其转过的圈数。 每当其重载后，cycle都会根据指令自增或者自减。这里，cycle理论上应该从0开始，但是为了能更方便的表达负数，这里将cycle的初值设置为127(**这就是为什么公式中会减去127**)
另一方面，cycle到底是增加还是减少，根据上位机发送的命令来判断
```
switch(RxMessage.Data[1])
        {
            case 1: cycle+=1; break;
            case 2: cycle-=1; break;
            case 3: cycle-=1; break;
            case 4: cycle+=1; break;
            default : cycle=cycle; break;
        }

```

这里 1  2分别表示直接控制前进和后退，所以cycle分别是增加和减少
这里 3  4分别表示，根据实际的里程数(ActDistance)和给予的里程数(distance)判断得出该增加或者减小(*正在设计当中*)

### 位置控制
位置控制方法中，主要借鉴何使用了S曲线加减速，对电机的启动、停止进行一个更加准确的把把控，从而提高对整个机器人的位置控制精度。

机器人行走精度的误差来源，主要在于对电机的控制信号消失后，其不能立即停下，天然的有一个缓冲区。 因此，在电机启动和停止时，对齐变化速度有一个掌控，即可提高其位置控制精度。S加减速控制曲线如下图所示。回归曲线的模型为
![](http://latex.codecogs.com/gif.latex?\\\y=(k-c)\times\frac{1}{1+e^{-a(x-b)}}+c)
![S加减速曲线](http://c.hiphotos.baidu.com/image/%70%69%63/item/4d086e061d950a7b8008e8aa06d162d9f2d3c99b.jpg)
在这里，我们使用的策略是，计算出每个时刻的转速，离散化后，将其PWM值存入数组，再在启动和停止时循环遍历和调取实现S曲线加速与减速。

## 通讯
通讯主要通过CAN总线，这里下位机有两块控制板，其发出的包头分别为
* 0x00CC0000 转台
* 0x00BB0000 运动控制

### 1. 控制指令
控制指令主要由上位机发送至下位机，下位机处于接收指令状态，根据不同指令来执行相应的动作

```
ID  =>  0x00AB0000  转台控制
  第一位  运动形式
  第二位  运动方向

  data= [0x0A 0X01] 转台正传
  data= [0x0A 0x02] 转台反转

  data [0x0C 0x01]  支撑杆伸长
  data [0x0C 0x02]  支撑杆收缩
```

```
ID => 0x00AA0000  运动控制

  第一位  0x0A 运动形式
  第二位  0x03||0x04 伺服运动   0x01||0x02 直接运动
    其中0x01是后退  0x02是前进
  第三位  私服运动下： 运动距离；
          直接运动下： 运动速度；
  第四位  在私服运动下才有效： 表示运动方向

  向前运动15cm：
    data = [0x0A 0x03 0x0F 0X00]
  先前全速运动：
    data = [0x0A 0x02 0x04]
```

### 2. 接收反馈指令
接收的反馈主要来自两个板子的**距离信息**反馈和**时间信息反馈**。即，当停止运动时，返回板子的运动距离；转台停止时，返回转台运动的时间。

