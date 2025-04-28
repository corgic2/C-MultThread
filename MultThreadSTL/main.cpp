#include <functional>
#include <iostream>
#include "Charpter2/CreateThread.h"

int main(int argc, char** argv)
{
    {
        TempWorkTask work;
        CreateThread charPter2;

        // 使用 std::bind 或 Lambda 表达式绑定成员函数
        charPter2.CreateThreadBJoin(true, std::bind(&TempWorkTask::doBaseThing, &work));
        charPter2.CreateThreadInVariableFunction(std::bind(&TempWorkTask::doVariableThing, &work, std::placeholders::_1));
        charPter2.CreateThreadByClassFunction();
        charPter2.CreateThreadCopyObject();
    }
    std::cout << "Main Function End" << std::endl;
    return 0;
}