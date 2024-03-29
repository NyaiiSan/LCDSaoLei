## C语言实现的扫雷游戏 

#### 硬件和系统
- CPU: IMX6ULL
- MEMORY: 512MB
- EMMC: 8.00GB
- RESOLUTION: 1024 x 600
- SYSTEM: Linux 4.1.15

#### 基本功能：

1. 拥有一个主菜单功能界面 能够通过点击区域来进入不同功能 `[√]`
2. 拥有不同的难度等级 通过点击选择 `[√]`  
   难度等级为扫雷区域大小不同 `[√]`
3. 插旗功能 `[√]`
4. 得分功能的实现和显示 `[√] 剩余地雷数量显示`
5. 成功与失败的提示 `[√] 成功或失败时全部地雷显示，并且计时暂停`
6. 可以重新开始游戏 `[√]`

#### 额外功能： 

1. 计时功能的实现和显示 `[√]`
2. 用户账户和密码机制  `[x] 不在计划之内`
3. 用户数据记录 如 胜场和败场记录 `[√] 记录完成的时间`
4. 用户胜场排行榜(可查询前10选手数据功能 最好能看自己的排名) `[O] 显示榜单`

#### 操作说明

- 编译运行  
   运行前，目录中需要存在`bg.bmp`文件作为背景显示  

- 开始游戏  
   进行游戏前，需要先选择游戏难度。游戏难度分为三个等级  
   - easy: 简单难度。9x9 的游戏区域大小，包含10个地雷  
   - normal: 普通难度。 16x16的游戏区域大小，包含40个地雷  
   - hard: 困难难度。22x22的游戏区域大小，包含99个地雷  

- 选择区域  
   游戏开始后，会在左侧绘制游戏区域，点击其中的一个未操作的格子，格子会变为选中状态，此时可对该格子进行打开或插旗操作。点击其他格子，会更换选中状态。

- 按键功能  
   右侧绘制三个点击按键。分别为打开、插旗、菜单
   - open: 打开格子。打开选中的格子，仅对未操作的格子有效。  
   - flag: 标记格子。对格子进行插旗操作，若格子已被插旗则取消插旗状态。  
   - menu: 打开菜单。打开游戏菜单选项，进行更多操作。  

- 菜单功能  
   打开菜单后可使用菜单功能。包括继续、重新开始、难度选择、游戏记录。
   - continue: 继续游戏。点击后关闭菜单，回到游戏。  
   - restart: 重新开始。点击后开始一轮新的游戏。  
   - difficulty: 难度选择。点击后选择更改游戏难度。  
   - players: 游玩记录。点击后显示游戏时间排行榜单，不同难度显示内容不同。  

- 计时功能  
   打开第一个格子后开始计时，成功或失败后计时暂停。重新开始游戏则会重置计时器。  

- 数量提示  
   显示剩余地雷数量，每次插旗操作(无论对错)都会改变数量显示。  

