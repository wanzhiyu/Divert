#pragma once
#include <wtypes.h>
#include <vector>
#include "url.h"
class Blacklist
{
public:
	bool PayloadMatch(const Url& url);
private:
	vector<Url*> urls;
};
