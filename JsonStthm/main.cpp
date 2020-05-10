
#define BENCHMARKER_USE_MACROS
#include "Benchmarker.h"

// JsonStthm
#include "JsonStthm.h"

// vivkin/gason
#include "gason.h"

// sheredom/JsonH
#include "json.h"

// hjiang/JsonXX
#include "jsonxx.h"
#include <fstream> // std::filebuf

// nlohmann/json
#include "nlohmann/json.hpp"

void main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Benchmarker_SetVerbose(true);

	BEGIN_TEST_SUITE("JsonStthm")
		const char* pFile = "../../Data/big.json";
		JsonStthm::JsonValue oValue;
		JsonStthm::JsonDoc oDoc;

		CHECK(oValue.GetType() == JsonStthm::JsonValue::E_TYPE_INVALID);
		CHECK(oValue.GetMemberCount() == 0)
		oValue[0].SetInteger(123);
		CHECK(oValue.GetMemberCount() == 1)
		CHECK(oValue[0].GetType() == JsonStthm::JsonValue::E_TYPE_INTEGER);


		CHECK(oValue.ReadFile(pFile) == 0)
		CHECK(oDoc.ReadFile(pFile) == 0)

		CHECK(oValue == oDoc.GetRoot())

		char* pJsonPretty = oDoc.GetRoot().WriteString(false);
		char* pJsonCompact = oDoc.GetRoot().WriteString(true);

		CHECK(pJsonPretty != NULL)
		CHECK(pJsonCompact != NULL)

		JsonStthm::JsonValue oValuePretty;
		JsonStthm::JsonValue oValueCompact;
		CHECK(oValuePretty.ReadString(pJsonPretty) == 0)
		CHECK(oValueCompact.ReadString(pJsonCompact) == 0)

		CHECK(oValue == oValuePretty)
		CHECK(oValue == oValueCompact)
	END_TEST_SUITE()

	Benchmarker_SetVerbose(false);

	BEGIN_BENCHMARK_VERSUS_WITH_ARG("Json parser", const char*,
		"../../Data/big.json",
		"../../Data/facebook.json",
		"../../Data/canada.json",
		"../../Data/jeopardy.json"
	)
		BEGIN_BENCHMARK_VERSUS_CHALLENGER("JsonStthm::JsonValue")
			JsonStthm::JsonValue oValue;
			CHECK(oValue.ReadFile(VERSUS_ARG) == 0);
		END_BENCHMARK_VERSUS_CHALLENGER()

		BEGIN_BENCHMARK_VERSUS_CHALLENGER("JsonStthm::JsonDoc")
			JsonStthm::JsonDoc oDoc;
			CHECK(oDoc.ReadFile(VERSUS_ARG) == 0);
		END_BENCHMARK_VERSUS_CHALLENGER()

		BEGIN_BENCHMARK_VERSUS_CHALLENGER("vivkin/gason")
			FILE* pFile = fopen(VERSUS_ARG, "rb");
			CHECK_FATAL(pFile != NULL)

			fseek(pFile, 0, SEEK_END);
			long iSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			char* pString = (char*)malloc(iSize);
			CHECK(pString != NULL);
			CHECK(fread(pString, 1, iSize, pFile) == iSize);
			fclose(pFile);
			char *endptr;
			::JsonValue value;
			JsonAllocator allocator;

			CHECK(JSON_OK == jsonParse(pString, &endptr, &value, allocator));
			free(pString);
		END_BENCHMARK_VERSUS_CHALLENGER()

		BEGIN_BENCHMARK_VERSUS_CHALLENGER("sheredom/JsonH")
			FILE* pFile = fopen(VERSUS_ARG, "rb");
			CHECK_FATAL(pFile != NULL)
			fseek(pFile, 0, SEEK_END);
			long iSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			char* pString = (char*)malloc(iSize);
			CHECK(pString != NULL);
			CHECK(fread(pString, 1, iSize, pFile) == iSize);
			fclose(pFile);

			json_value_s* pValue = json_parse(pString, iSize);

			CHECK(pValue != NULL)

			free(pValue);
			free(pString);
		END_BENCHMARK_VERSUS_CHALLENGER()

		BEGIN_BENCHMARK_VERSUS_CHALLENGER("hjiang/JsonXX")
			std::filebuf oFileBuf;
			CHECK_FATAL(oFileBuf.open(VERSUS_ARG, std::ios::in) != NULL)
			std::istream oIStream(&oFileBuf);
			jsonxx::Object oJson;
			CHECK(oJson.parse(oIStream))
			oFileBuf.close();
		END_BENCHMARK_VERSUS_CHALLENGER()

		BEGIN_BENCHMARK_VERSUS_CHALLENGER("nlohmann/json")
			std::filebuf oFileBuf;
			CHECK_FATAL(oFileBuf.open(VERSUS_ARG, std::ios::in) != NULL)
			bool bOk = true;
			std::istream oIStream(&oFileBuf);
			nlohmann::json oJson;
			CHECK_TRY(oIStream >> oJson);
			oFileBuf.close();
		END_BENCHMARK_VERSUS_CHALLENGER()
	END_BENCHMARK_VERSUS_ARGS()

	//system("pause");
}
