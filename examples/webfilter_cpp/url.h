#pragma once

#include <string>
using namespace std;

class Url
{
public:
	Url(const string& domain, const string& uri) :domain(domain), uri(uri) {}
	string getDomain() const { return this->domain; }
private:
	string domain;
	string uri;
};
