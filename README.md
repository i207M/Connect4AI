# 重力四子棋 Connect4

使用信心上限树算法，并没有加入什么 Trick，就实现了 97%的胜率。

![image-20230604223843588](https://img.i207m.top/2023/06_6472cf34a06bf4d5d64c1ff6c53f4896.png)

## 游戏规则

游戏在一个宽高在[9,12]内随机的棋盘上进行，并有一个随机点不能落子。每个玩家轮流在棋盘上放置棋子，每次只能在列顶落子。当一方在横、竖、斜方向上有四个棋子连成一线时，该方获胜。每一步的计算时间为 3s。

## 代码结构

基于面向对象的思想，我将算法的关键部分实现为三个类，它们分别是：

- State：储存棋盘的状态，支持扩展到下一个状态，实现了 DefaultPolicy 方法
- TreeNode：蒙特卡洛树的节点，对应一个棋盘的状态，记录的树的结构（儿子兄弟表示法），实现了 BestChild 方法
- UCT：包含 root 节点，实现了 TreePolicy 和 Search 方法

## 改进

虽然我尝试引入了各种先验（棋盘中部的权重更大、必胜态与必败态等），它们或多或少有一些影响，但是我认为，simple is best，因此最终保留了 Vanilla 的 UCT 算法。

## 参考

感谢 [qibinc](https://github.com/qibinc)/[Connect-Four](https://github.com/qibinc/Connect-Four) 和 [zhaochenyang20](https://github.com/zhaochenyang20)/[IAI_2022](https://github.com/zhaochenyang20/IAI_2022)。
