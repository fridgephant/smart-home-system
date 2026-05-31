# 基于ESP32的智能家居环境监测与控制系统

随着物联网技术成熟和用户对居住环境品质要求提升，本文设计并实现了一套基于ESP32的智能家居环境监测与控制系统。系统以ESP32-WROOM-32E为核心，融合温湿度、光照、空气质量和OLED显示模块，实现“采集—传输—判断—控制”的智能闭环。

## 系统功能
- 实时采集室内环境参数：温度、湿度、光照强度、TVOC、CO₂当量
- 本地显示环境数据：0.96英寸OLED屏幕显示当前参数
- 无线传输数据：ESP32内置Wi-Fi通过MQTT将数据上传至云服务器
- APP远程查看：用户可在手机APP查看实时数据并设置报警阈值
- 自动设备联动：温度超标启风扇、光照不足启补光灯、空气质量超标启净化器
- 手动控制接口：远程APP可切换继电器状态，实现手动控制

## 目录说明
- `src/`：主程序代码，基于Arduino框架实现
- `inc/`：头文件与系统参数定义
- `build/`：编译输出目录
- `tests/`：Monte Carlo测试脚本
- `.vscode/`：VS Code任务配置
- `Makefile`：构建说明与编译命令

## 开发环境
- ESP32-WROOM-32E芯片
- Arduino开发框架
- SHT30温湿度传感器
- BH1750光照传感器
- SGP30空气质量传感器
- 0.96英寸OLED显示屏
- MQTT云服务器与手机APP

## 编译说明
1. 安装 Arduino CLI：
   ```bash
   brew install arduino-cli
   ```
2. 安装 ESP32 开发板支持：
   ```bash
   arduino-cli core update-index
   arduino-cli core install esp32:esp32
   ```
3. 安装库依赖：
   ```bash
   arduino-cli lib install "Adafruit SHT31" "BH1750" "Adafruit SGP30" "Adafruit SSD1306" "Adafruit GFX Library" "PubSubClient"
   ```
4. 打开终端，进入项目根目录
5. 运行：
   ```bash
   make
   ```
6. 生成的固件文件会存放在 `build/` 目录

## 测试方案
### 1. 功能测试
- 在ESP32上运行程序，观察OLED显示的温湿度、光照和空气质量数据
- 通过手机APP查看MQTT上传数据是否正确
- 改变环境参数，检查风扇、补光灯、空气净化器是否自动联动

### 2. Monte Carlo随机测试
- 已增加Monte Carlo统计脚本，用于评估控制阈值合理性
- 脚本随机生成大量环境参数样本，统计自动控制触发概率
- 运行方式：
  ```bash
  python3 tests/monte_carlo_test.py
  ```
- 输出结果包括：风扇启动比例、补光灯启动比例、空气净化器启动比例

### 3. 系统验证
- 验证环境采集精度是否满足家居监测要求
- 验证MQTT通信是否稳定可靠
- 验证自动控制是否响应准确

## 运行步骤
1. 编译固件：`make`
2. 将 `build/firmware.bin` 烧录到ESP32
3. 接入SHT30、BH1750、SGP30、OLED和继电器模块
4. 配置Wi-Fi和MQTT服务器
5. 通过APP检查云端数据与设备控制

## 备注
本项目已实现基础智能家居环境监测与联动控制方案，后续可继续扩展语音控制、更多传感器、历史数据存储和更完善的远程APP交互。