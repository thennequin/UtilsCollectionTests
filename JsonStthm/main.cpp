

#include <Windows.h>

#include <stdio.h>

#include "JsonStthm.h"

//#include "jsonxx.h"

#include "gason.h"

uint64_t nanotime() {
#if defined(__linux__)
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * UINT64_C(1000000000) + ts.tv_nsec;
#elif defined(__MACH__)
	static mach_timebase_info_data_t info;
	if (info.denom == 0)
		mach_timebase_info(&info);
	return mach_absolute_time() * info.numer / info.denom;
#elif defined(_WIN32)
	static LARGE_INTEGER frequency;
	if (frequency.QuadPart == 0)
		QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart * UINT64_C(1000000000) / frequency.QuadPart;
#endif
}

bool StthmTestFile(const char* pFilePath)
{
	JsonStthm::JsonValue oValue;
	int iLineError = oValue.ReadFile(pFilePath);
	return iLineError == 0;
}

bool JsonXXTestFile(const char* pFile)
{
	return false;
}

bool GasonTestFile(const char* pFilePath)
{
	FILE* pFile = fopen(pFilePath, "r");
	if (pFile != NULL)
	{
		fseek(pFile, 0, SEEK_END);
		long iSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		char* pString = new char[iSize / sizeof(char)];
		fread(pString, 1, iSize, pFile);
		fclose(pFile);
		char *endptr;
		::JsonValue value;
		JsonAllocator allocator;
		int status = jsonParse(pString, &endptr, &value, allocator);
		bool bOk = JSON_OK == status;
		delete pString;
		return bOk;
	}
	return false;
}


void LaunchTest(const char* pName, bool(*pFunction)(const char*), const char* pFile)
{
	uint64_t iStartTime = nanotime();
	bool bOk = pFunction(pFile);
	uint64_t iEndTime = nanotime();
	printf("%s : %s\n\t time %d ns\n", pName, bOk ? "Ok" : "Fail", (iEndTime - iStartTime));
}

void main()
{
	const char* pFile = "../../Data/big.json";

	LaunchTest("Gason", GasonTestFile, pFile);
	LaunchTest("JsonStthm", StthmTestFile, pFile);

	JsonStthm::JsonValue oValue;
	JsonStthm::JsonValue& oArray = oValue["myArray"];
	oArray[0] = "test";
	oArray[1] = false;
	oArray[2] = 3.14159265359f;

	JsonStthm::String sOut;
	oValue.WriteString(sOut);

	system("pause");
}