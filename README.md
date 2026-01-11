# SunThemeSwitcher

一个基于**日出 / 日落时间**，自动切换 **Wallpaper Engine** 播放列表的 Windows 后台工具。

SunThemeSwitcher 会根据你所在地的真实日出、日落时间，在后台自动切换 Wallpaper Engine 的播放列表，实现**白天 / 夜间壁纸主题的自动切换**。  
一次配置，长期运行。

---

## 功能特性

- 基于本地日出 / 日落时间自动切换播放列表  
- 控制 Wallpaper Engine 播放列表（调用 `wallpaper64.exe`）  
- 支持开机自启动（可配置）  
- 首次运行自动创建配置文件  
- 自动检测并修复 Wallpaper Engine 路径  
- 后台静默运行（Release 版本无窗口）  
- 防止多实例运行  
- 支持通过配置文件触发卸载并自动清理  

---

## 工作原理

程序会周期性检测当前时间状态：

- **日出（Sunrise）** → 切换到白天播放列表  
- **日落（Sunset）** → 切换到夜间播放列表  

内部通过向 `wallpaper64.exe` 传递控制参数，实现对 Wallpaper Engine 的远程控制。

---

## 使用说明

### 1. 首次运行

第一次运行程序时：

1. 在 `%AppData%\SunThemeSwitcher` 下创建配置文件  
2. 检测到配置不完整  
3. **自动打开配置文件**
4. 程序退出  

这是**预期行为**，请按提示完成配置。

---

### 2. 配置文件

配置文件路径：%AppData%\SunThemeSwitcher\config.ini


程序在**首次运行**时会自动创建该文件（如果不存在），并在配置不完整的情况下自动打开该文件供用户编辑。

---

## 配置文件示例

```ini
playlist_white=DayPlaylist
playlist_black=NightPlaylist
wallpaper_path=C:\Program Files (x86)\Steam\steamapps\common\wallpaper_engine\wallpaper64.exe
autorun=true
autorun_set=true

## 配置项详解

以下内容详细说明 `%AppData%\SunThemeSwitcher\config.ini` 中各个配置项的含义、取值要求及程序行为。

---

### playlist_white

- **类型**：字符串  
- **必填**：是  
- **含义**：  
  日出后（白天）使用的 Wallpaper Engine 播放列表名称。

- **说明**：  
  - 必须与 Wallpaper Engine 内显示的播放列表名称**完全一致**
  - 区分大小写
  - 不能为空

示例：

```ini
playlist_white=DayPlaylist

### playlist_black

- **类型**：字符串  
- **必填**：是  

- **含义**：  
  日落后（夜间）使用的 Wallpaper Engine 播放列表名称。

- **说明**：  
  - 必须与 Wallpaper Engine 中显示的播放列表名称**完全一致**  
  - 区分大小写  
  - 不能为空  

- **示例**：

```ini
playlist_black=NightPlaylist

### wallpaper_path

- **类型**：字符串（可执行文件完整路径）  
- **必填**：是（支持自动修复）  

- **含义**：  
  Wallpaper Engine 主程序 `wallpaper64.exe` 的完整路径。

- **行为说明**：  
  - 程序启动时会自动尝试查找 Wallpaper Engine 的安装目录  
  - 当配置文件中该项为空时，若自动检测成功，将自动写入配置文件  
  - 若自动检测失败，则需要用户手动填写  

- **示例**：

```ini
wallpaper_path=C:\Program Files (x86)\Steam\steamapps\common\wallpaper_engine\wallpaper64.exe

### autorun

- **类型**：布尔值（true / false）  
- **必填**：否  

- **含义**：  
  控制程序是否在 Windows 启动时自动运行。

- **取值说明**：

| 值 | 行为 |
|----|------|
| true | 程序将被加入开机自启动 |
| false | 程序不会随系统启动 |

- **示例**：

```ini
autorun=true


## 配置完整性判定

程序在启动时会对 `%AppData%\SunThemeSwitcher\config.ini` 进行完整性检查，以下任意情况将被判定为**配置不完整**：

- 配置文件不存在  
- `playlist_white` 为空  
- `playlist_black` 为空  
- `wallpaper_path` 为空且无法自动检测  

---

### 配置不完整时的行为

当检测到配置不完整时，程序将严格按照以下流程执行：

1. 若配置文件不存在，则自动创建配置文件模板  
2. 弹出提示窗口，告知用户配置尚未完成  
3. 自动打开配置文件供用户编辑  
4. 程序立即退出，不进入主循环、不驻留后台  

用户完成配置并保存文件后，需要**手动重新启动程序**。

---

## 卸载配置（uninstall）

如果在 `config.ini` 的**最后一行**单独写入以下内容：

```ini
uninstall
并运行程序一次，程序将进入自卸载流程。

### 卸载行为说明

当程序检测到配置文件 `config.ini` 的**最后一行**为：

```ini
uninstall
并运行程序一次，程序将进入自卸载流程。

---

### 自卸载流程说明

当满足卸载条件后，程序启动即进入卸载模式，不会执行任何壁纸切换或后台逻辑。自卸载流程按以下顺序执行：

1. 注销已注册的 Windows 开机自启动项  
2. 删除整个配置目录及其内容：%AppData%\SunThemeSwitcher\

3. 删除程序自身的可执行文件（`SunThemeSwitcher.exe`）  
4. 程序自动退出  

卸载完成后，系统中将不会残留任何相关文件或后台进程。

---

### 注意事项

- `uninstall` 必须位于 `config.ini` 的**最后一行**，且独占一行  
- 卸载时请确保程序未被其他进程占用  
- 若卸载过程中删除失败，可在程序退出后手动清理残留文件  
- 卸载操作不可逆，请确认后再执行  

---

