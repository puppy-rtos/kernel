# 设计文档

## 启动流程

### 需求

1. 支持使用宏的方式初始化线程，并自动启动，或延时启动。
2. 清晰化，简化流程，方便用户理解和定位问题
3. 兼顾多核启动

#### 多核启动流程

```flow
st=>start: 系统上电
bare_metal=>operation: 裸机环境
cpu0=>condition: cpu0?
puppy_init=>operation: puppy初始化
wfe=>operation: 等待唤醒
e=>end: 开始调度

st->bare_metal->cpu0
cpu0(yes)->puppy_init->e
cpu0(no)->wfe->e
```

#### puppy初始化

```flow
s=>start: main
hw_init=>operation: 硬件初始化
puppy=>operation: puppy_init
inter=>operation: 内部初始化
thread=>operation: 宏静态线程列表初始化
start=>operation: puppy_start
schd=>operation: 调度器启动
init=>operation: init函数列表初始化（init线程）

s->hw_init->puppy(bottom)->start->schd(right)->init

```

```flow
s=>start: main
hw_init=>operation: 硬件初始化
puppy=>operation: puppy_init
inter=>operation: 内部初始化
thread=>operation: 宏静态线程列表初始化
start=>operation: puppy_start
schd=>operation: 调度器启动
init=>operation: init函数列表初始化（init线程）


puppy->inter->thread
```

