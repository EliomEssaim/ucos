代码中使用到的需要在工程文件中添加的宏定义有：
1、UNIT_TEST
用于最早的CPP_UNIT单元测试，包含了单元测试代码，VC工程中使用

2、M3
用于配合M3进行的集成测试，包含集成测试代码，BCB工程中使用

3、BIG_ENDIAN
为了防止CPU的大、小端模式变化而定义的，目前没有使用

4、YK_POI
用于POI机型，其代码和通用机型不同，使用宏开关进行切换，英蓓特工程中使用

5、YK_DEBUG
在DEBUG工程中定义，如果没有定义就是RELEASE工程，它们区别在于前者不打开看门狗。AppD.pjf是DEBUG工程文件，App.pjf是RELEASE工程文件，前者定义了YK_DEBUG