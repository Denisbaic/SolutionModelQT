#include "Generator.h"
#include <cmath>
#include <ctime>



Generator::Generator(unsigned long _HiValue, unsigned long _LoValue): m(std::pow(2, 64) - 1), HiValue(_HiValue), LoValue(_LoValue), CurrentValue((unsigned long long int)(time(NULL)) % _HiValue+ 1)
{
}

Generator::~Generator()
{
}

unsigned int Generator::next() // функция генерации псевдослучайных чисел
{
	CurrentValue = a * CurrentValue % m;
	return CurrentValue% (HiValue- LoValue+1)+LoValue;
}
