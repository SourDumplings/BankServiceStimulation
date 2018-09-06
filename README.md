# BankServiceStimulation

----- 版权所有 酸饺子 -----

银行服务模拟程序
模拟银行排队服务的场景，可以选择模拟以下特殊附加条件下的情况之一：

    1.有VIP服务
    VIP在队列中，并且VIP窗口空闲时会优先服务VIP。即排在最前面的VIP会直接到编号最小的VIP窗口进行服务。

    2.有黄线
    银行排队处有一条黄线，黄线以内是每个窗口自己的队列（用容量限制）
    每个窗口为自己在黄线以内队列中最前面的客户提供服务。黄线以外是统一的队列。
    客户刚来银行如果黄线以内有空位，总会选择最短的队列，否则就只能站在黄线以外排队。

    3.好友夹塞
    如果A认识B并且B认识A，并且A已经在队列中但是并未开始进行服务。
    那么在B刚来银行时便可以不用在最后排队而是直接插队站在A的后面。

目前已完成1,2，在visio studio 2017下测试通过

