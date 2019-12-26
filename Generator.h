#pragma once

class Generator
{
	const unsigned long long  m;
	const unsigned long long  a = 6364136223846793005;
	unsigned long HiValue;
	unsigned long LoValue;
public:
	unsigned int CurrentValue;
	//Generator();
	Generator(unsigned long _HiValue, unsigned long LoValue);
	~Generator();
	unsigned int next();
};

