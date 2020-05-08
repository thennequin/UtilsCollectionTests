

#include <Windows.h>

#include <stdio.h>

#include "JsonStthm.h"

#include <string>

#include "jsonxx.h"
#include <fstream> // std::filebuf

#include "gason.h"

#include "json.h"

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

bool StthmDocTestFile(const char* pFilePath)
{
	JsonStthm::JsonDoc oDoc;
	int iLineError = oDoc.ReadFile(pFilePath);
	return iLineError == 0;
}

bool JsonXXTestFile(const char* pFile)
{
	std::filebuf oFileBuf;
	if (oFileBuf.open(pFile, std::ios::in))
	{
		std::istream oIStream(&oFileBuf);
		jsonxx::Object oJson;
		bool bOk = oJson.parse(oIStream);
		oFileBuf.close();
		return bOk;
	}

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

bool JsonHTestFile(const char* pFilePath)
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

		json_value_s* pValue = json_parse(pString, iSize);

		bool bOk = pValue != NULL;

		delete pString;
		return bOk;
	}
	return false;
}


void GetReadableSize(int iSize, char* pBuffer, int iBufferSize)
{
	static const char* const pSizes[] = { "bytes", "Kb", "Mb", "Gb" };
	int iDiv = 0;
	int iRem = 0;

	while (iSize >= 1024 && iDiv < (sizeof pSizes / sizeof *pSizes)) {
		iRem = (iSize % 1024);
		iDiv++;
		iSize /= 1024;
	}

	snprintf(pBuffer, iBufferSize, "%.1f %s\n", (float)iSize + (float)iRem / 1024.0, pSizes[iDiv]);
}

void GetReadableTime(uint64_t iNanoseconds, char* pBuffer, int iBufferSize)
{
	static const char* const pTimes[] = { "ns", "us", "ms", "s" };
	int iDiv = 0;
	int iRem = 0;

	while (iNanoseconds >= 1000 && iDiv < (sizeof pTimes / sizeof *pTimes)) {
		iRem = (iNanoseconds % 1000);
		iDiv++;
		iNanoseconds /= 1000;
	}

	snprintf(pBuffer, iBufferSize, "%.3f %s\n", (float)iNanoseconds + (float)iRem / 1000.0, pTimes[iDiv]);
}

void LaunchTest(const char* pName, bool(*pFunction)(const char*), const char* pFile)
{
	uint64_t iStartTime = nanotime();
	bool bOk = pFunction(pFile);
	uint64_t iEndTime = nanotime();

	char pBuffer[256];
	GetReadableTime(iEndTime - iStartTime, pBuffer, 256);
	printf("%s : %s\n\t time %s\n", pName, bOk ? "Ok" : "Fail", pBuffer);
}

void main()
{
	const char* pFile = "../../Data/big.json";

	LaunchTest("JsonStthm::JsonValue", StthmTestFile, pFile);
	LaunchTest("JsonStthm::JsonDoc", StthmDocTestFile, pFile);
	LaunchTest("Gason", GasonTestFile, pFile);
	LaunchTest("JsonXX", JsonXXTestFile, pFile);
	LaunchTest("Json.h", JsonHTestFile, pFile);

	JsonStthm::JsonValue oValue;
	JsonStthm::JsonValue& oArray = oValue["myArray"];
	oArray[0] = "test";
	oArray[1] = false;
	oArray[2] = 3.14159265359f;

	std::string sOut;
	oValue.WriteString(sOut);

	system("pause");
}