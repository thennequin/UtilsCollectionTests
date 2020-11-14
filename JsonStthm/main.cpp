
#define BENCHMARKER_USE_MACROS
#include "Benchmarker.h"

// JsonStthm
#include "JsonStthm.h"

// vivkin/gason
#include "gason.h"

// sheredom/JsonH
namespace sheredom
{
#include "json.h/json.h"
}

// hjiang/JsonXX
#include "jsonxx.h"
#include <fstream> // std::filebuf

// nlohmann/json
#include "nlohmann/json.hpp"

// vincenthz/libjson
namespace vincenthz
{
#include "vincenthz-libjson/json.h"
}

void main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Benchmarker_SetVerbose(true);

	BEGIN_TEST_SUITE("JsonStthm");
	{
		BEGIN_TEST_SUITE("Type parser");
		{
			JsonStthm::JsonValue oValue;

			BEGIN_TEST_SUITE("Null");
			{
				CHECK_FATAL(oValue.ReadString("null") == 0);
				CHECK_FATAL(oValue.IsNull());
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Boolean true");
			{
				CHECK_FATAL(oValue.ReadString("true") == 0);
				CHECK_FATAL(oValue.IsBoolean());
				CHECK_FATAL(oValue.ToBoolean() == true);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Boolean false");
			{
				CHECK_FATAL(oValue.ReadString("false") == 0);
				CHECK_FATAL(oValue.IsBoolean());
				CHECK_FATAL(oValue.ToBoolean() == false);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Float NaN");
			{
				CHECK_FATAL(oValue.ReadString("NaN") == 0);
				CHECK_FATAL(oValue.IsFloat());
				CHECK_FATAL(JsonStthm::Internal::IsNaN(oValue.ToFloat()));
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Float Infinity positive");
			{
				CHECK_FATAL(oValue.ReadString("Infinity") == 0);
				CHECK_FATAL(oValue.IsFloat());
				CHECK_FATAL(JsonStthm::Internal::IsInfinite(oValue.ToFloat()));
				CHECK_FATAL(oValue.ToFloat() > 0.0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Float Infinity negative");
			{
				CHECK_FATAL(oValue.ReadString("-Infinity") == 0);
				CHECK_FATAL(oValue.IsFloat());
				CHECK_FATAL(JsonStthm::Internal::IsInfinite(oValue.ToFloat()));
				CHECK_FATAL(oValue.ToFloat() < 0.0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("String : Empty");
			{
				CHECK_FATAL(oValue.ReadString("\"\"") == 0);
				CHECK_FATAL(oValue.IsString());
				CHECK_FATAL(strcmp(oValue.ToString(), "") == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("String : Simple text");
			{
				CHECK_FATAL(oValue.ReadString("\"Hello World!\"") == 0);
				CHECK_FATAL(oValue.IsString());
				CHECK_FATAL(strcmp(oValue.ToString(), "Hello World!") == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("String : Escaped chars");
			{
				CHECK_FATAL(oValue.ReadString("\" \\n \\r \\t \\b \\f \\\\ \\/ \"") == 0);
				CHECK_FATAL(oValue.IsString());
				CHECK_FATAL(strcmp(oValue.ToString(), " \n \r \t \b \f \\ \/ ") == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("String : UTF8 character (paragraph sign)");
			{
				CHECK_FATAL(oValue.ReadString("\" \\u00B6 \\u00b6 \"") == 0);
				CHECK_FATAL(oValue.IsString());
				CHECK_FATAL(strcmp(oValue.ToString(), " \xC2\xB6 \xC2\xB6 ") == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("String : UTF8 character with UTF16 surrogate (Ace of Spades)");
			{
				CHECK_FATAL(oValue.ReadString("\" \\ud83c\\udca1 \"") == 0);
				CHECK_FATAL(oValue.IsString());
				CHECK_FATAL(strcmp(oValue.ToString(), " \xF0\x9F\x82\xA1 ") == 0);
			}
			END_TEST_SUITE();

			{
				CHECK_FATAL(oValue.ReadString("[]") == 0);
				CHECK_FATAL(oValue.IsArray());
				CHECK_FATAL(oValue.GetMemberCount() == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Object empty");
			{
				CHECK_FATAL(oValue.ReadString("{}") == 0);
				CHECK_FATAL(oValue.IsObject());
				CHECK_FATAL(oValue.GetMemberCount() == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Array of empty Object");
			{
				CHECK_FATAL(oValue.ReadString("[{}, {}]") == 0);
				CHECK_FATAL(oValue.IsArray());
				CHECK_FATAL(oValue[0].IsObject());
				CHECK_FATAL(oValue[0].GetMemberCount() == 0);
				CHECK_FATAL(oValue[1].IsObject());
				CHECK_FATAL(oValue[1].GetMemberCount() == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Invalid object");
			{
				CHECK_FATAL(oValue.ReadString("{ \"name\" }") != 0);
				CHECK_FATAL(oValue.ReadString("{ \"name\" : }") != 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Object with numeric");
			{
				CHECK_FATAL(oValue.ReadString("{\"a\" : 42}") == 0);
				CHECK_FATAL(oValue.IsObject());
				CHECK_FATAL(oValue["a"].IsNumeric());
				CHECK_FATAL(oValue["a"].ToInteger() == 42);
			}
			END_TEST_SUITE();
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE("JsonValue creation")
		{
			JsonStthm::JsonValue oValue;
			CHECK(oValue.GetType() == JsonStthm::JsonValue::E_TYPE_INVALID);
			CHECK(oValue.GetMemberCount() == 0);
			oValue[0].SetInteger(123);
			CHECK(oValue.GetMemberCount() == 1);
			CHECK(oValue[0].GetType() == JsonStthm::JsonValue::E_TYPE_INTEGER);
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE("JsonValue/JsonDoc equality")
		{
			JsonStthm::JsonValue oValue;
			JsonStthm::JsonDoc oDoc;

			const char* pFile = "../../Data/big.json";
			CHECK(oValue.ReadFile(pFile) == 0);
			CHECK(oDoc.ReadFile(pFile) == 0);

			CHECK(oValue == oDoc.GetRoot());

			char* pJsonPretty = oValue.WriteString(false);
			char* pJsonCompact = oValue.WriteString(true);

			CHECK(pJsonPretty != NULL);
			CHECK(pJsonCompact != NULL);

			JsonStthm::JsonValue oValuePretty;
			JsonStthm::JsonValue oValueCompact;
			CHECK(oValuePretty.ReadString(pJsonPretty) == 0);
			CHECK(oValueCompact.ReadString(pJsonCompact) == 0);

			CHECK(oValue == oValuePretty);
			CHECK(oValue == oValueCompact);
		}
		END_TEST_SUITE();
	}
	END_TEST_SUITE();

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

			sheredom::json_value_s* pValue = sheredom::json_parse(pString, iSize);

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

		BEGIN_BENCHMARK_VERSUS_CHALLENGER("vincenthz/libjson")
			FILE* pFile = fopen(VERSUS_ARG, "rb");
			CHECK_FATAL(pFile != NULL)
			fseek(pFile, 0, SEEK_END);
			long iSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);

			char* pString = (char*)malloc(iSize);
			CHECK(pString != NULL);
			CHECK(fread(pString, 1, iSize, pFile) == iSize);
			fclose(pFile);

			vincenthz::json_parser oParser;
			CHECK(vincenthz::json_parser_init(&oParser, NULL, NULL, NULL) == 0)
			CHECK(vincenthz::json_parser_string(&oParser, pString, iSize, NULL) == 0)
			CHECK(vincenthz::json_parser_is_done(&oParser))
			CHECK(json_parser_free(&oParser) == 0)
		END_BENCHMARK_VERSUS_CHALLENGER()
	END_BENCHMARK_VERSUS_ARGS()

	//system("pause");
}
