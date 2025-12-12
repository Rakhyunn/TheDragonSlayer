#include "InstancePortAllocator.h"

void UInstancePortAllocator::Init(int32 InStart, int32 InEnd)
{
    Start = InStart;
    End = InEnd;
    Next = Start;
}

bool UInstancePortAllocator::TryAcquire(int32& OutPort)
{
    const int32 Span = End - Start + 1;
    for (int i = 0; i < Span; i++)
    {
        const int32 P = Next;
        Next = (Next + 1 > End) ? Start : Next + 1;
        if (!Used.Contains(P))
        { 
            Used.Add(P); 
            OutPort = P; 
            return true;
        }
    }
    return false;
}

void UInstancePortAllocator::Release(int32 Port)
{
    Used.Remove(Port);
}
