
#define BENCHMARKER_USE_MACROS
#include "Benchmarker.h"

// JsonStthm
#include "JsonStthm.h"

#define INCLUDE_TEST_GASON
#define INCLUDE_TEST_SHEREDOM
#define INCLUDE_TEST_HJIANG
#define INCLUDE_TEST_NLOHMANN
#define INCLUDE_TEST_VINCENTHZ

#ifdef INCLUDE_TEST_GASON
// vivkin/gason
#include "gason.h"
#endif // INCLUDE_TEST_GASON

#ifdef INCLUDE_TEST_SHEREDOM
// sheredom/JsonH
namespace sheredom
{
#include "json.h/json.h"
}
#endif // INCLUDE_TEST_SHEREDOM

#ifdef INCLUDE_TEST_HJIANG
// hjiang/JsonXX
#include "jsonxx.h"
#include <fstream> // std::filebuf
#endif // INCLUDE_TEST_HJIANG

#ifdef INCLUDE_TEST_NLOHMANN
// nlohmann/json
#include "nlohmann/json.hpp"
#endif // INCLUDE_TEST_NLOHMANN

#ifdef INCLUDE_TEST_VINCENTHZ
// vincenthz/libjson
namespace vincenthz
{
#include "vincenthz-libjson/json.h"
}
#endif // INCLUDE_TEST_VINCENTHZ

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

			BEGIN_TEST_SUITE("Boolean : true");
			{
				CHECK_FATAL(oValue.ReadString("true") == 0);
				CHECK_FATAL(oValue.IsBoolean());
				CHECK_FATAL(oValue.ToBoolean() == true);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Boolean : false");
			{
				CHECK_FATAL(oValue.ReadString("false") == 0);
				CHECK_FATAL(oValue.IsBoolean());
				CHECK_FATAL(oValue.ToBoolean() == false);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Float : NaN");
			{
				CHECK_FATAL(oValue.ReadString("NaN") == 0);
				CHECK_FATAL(oValue.IsFloat());
				CHECK_FATAL(JsonStthm::Internal::IsNaN(oValue.ToFloat()));
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Float : Infinity positive");
			{
				CHECK_FATAL(oValue.ReadString("Infinity") == 0);
				CHECK_FATAL(oValue.IsFloat());
				CHECK_FATAL(JsonStthm::Internal::IsInfinite(oValue.ToFloat()));
				CHECK_FATAL(oValue.ToFloat() > 0.0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Float : Infinity negative");
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

			BEGIN_TEST_SUITE("Array : empty");
			{
				CHECK_FATAL(oValue.ReadString("[]") == 0);
				CHECK_FATAL(oValue.IsArray());
				CHECK_FATAL(oValue.GetMemberCount() == 0);
			}
			END_TEST_SUITE();

			BEGIN_TEST_SUITE("Object : empty");
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
			CHECK(oValue.GetType() == JsonStthm::JsonValue::E_TYPE_NULL);
			CHECK(oValue.GetMemberCount() == 0);
			oValue[0].SetInteger(123);
			CHECK(oValue.GetMemberCount() == 1);
			CHECK(oValue[0].GetType() == JsonStthm::JsonValue::E_TYPE_INTEGER);
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE("JsonValue Copy")
		{
			JsonStthm::JsonValue oLeft, oRight;

			// Integer
			oRight.SetInteger(24);

			oLeft = oRight;
			CHECK(oLeft.IsInteger() && oLeft.ToInteger() == 24);
			
			oLeft.SetInteger(42);
			oLeft = oRight;
			CHECK(oLeft.IsInteger() && oLeft.ToInteger() == 24);

			// Float
			oRight.SetFloat(24.f);

			oLeft = oRight;
			CHECK(oLeft.IsFloat() && oLeft.ToInteger() == 24.f);

			oLeft.SetFloat(42.f);
			oLeft = oRight;
			CHECK(oLeft.IsFloat() && oLeft.ToInteger() == 24.f);

			// String
			oRight.SetString("Hello");

			oLeft = oRight;
			CHECK(oLeft.IsString() && strcmp(oLeft.ToString(), "Hello") == 0);

			oLeft.SetString("World");
			oLeft = oRight;
			CHECK(oLeft.IsString() && strcmp(oLeft.ToString(), "Hello") == 0);

			// Array
			oRight.InitType(JsonStthm::JsonValue::E_TYPE_ARRAY);
			oRight.Append().SetInteger(42);
			oRight.Append().SetString("Hello");

			oLeft = oRight;
			CHECK(oLeft.IsArray() && oLeft.GetMemberCount() == 2 && oLeft[0].IsInteger() && oLeft[0].ToInteger() == 42 && oLeft[1].IsString() && strcmp(oLeft[1].ToString(), "Hello") == 0);

			oLeft.InitType(JsonStthm::JsonValue::E_TYPE_ARRAY);
			oLeft.Append().SetInteger(24);
			oLeft = oRight;
			CHECK(oLeft.IsArray() && oLeft.GetMemberCount() == 2 && oLeft[0].IsInteger() && oLeft[0].ToInteger() == 42 && oLeft[1].IsString() && strcmp(oLeft[1].ToString(), "Hello") == 0);

			// Object
			oRight.InitType(JsonStthm::JsonValue::E_TYPE_OBJECT);
			oRight["First"].SetInteger(42);
			oRight["Second"].SetString("Hello");

			oLeft = oRight;
			CHECK(oLeft.IsObject() && oLeft.GetMemberCount() == 2 && oLeft[0].IsInteger() && oLeft[0].ToInteger() == 42 && oLeft[1].IsString() && strcmp(oLeft[1].ToString(), "Hello") == 0);

			oLeft.InitType(JsonStthm::JsonValue::E_TYPE_OBJECT);
			CHECK(oLeft.IsObject() && oLeft.GetMemberCount() == 0);
			oLeft["Third"].SetInteger(24);
			oLeft = oRight;
			CHECK(oLeft.IsObject() && oLeft.GetMemberCount() == 2 && oLeft[0].IsInteger() && oLeft[0].ToInteger() == 42 && oLeft[1].IsString() && strcmp(oLeft[1].ToString(), "Hello") == 0);
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE( "JsonValue Array Append" )
		{
			JsonStthm::JsonValue oValue;
			oValue = "test";
			// Disallow Append on string
			CHECK(oValue.Append().IsValid() == false);

			oValue.Reset();
			// Allow Append on type Null
			CHECK(oValue.Append().IsValid());

			// Allow Append on type Array
			CHECK(oValue.Append().IsValid());
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE("JsonValue Combine")
		{
			JsonStthm::JsonValue oLeft, oRight, oCombined;
			
			// Integer
			oLeft.SetInteger(42);
			oRight.SetInteger(24);
			oCombined = oLeft;
			CHECK(oCombined.Combine(oRight, false));
			CHECK(oCombined.IsInteger());
			CHECK(oCombined.IsInteger() && oCombined.ToInteger() == 66);

			// Float
			oLeft.SetFloat(42.5f);
			oRight.SetFloat(24.25f);
			oCombined = oLeft;
			CHECK(oCombined.Combine(oRight, false));
			CHECK(oCombined.IsFloat());
			CHECK(oCombined.IsFloat() && oCombined.ToFloat() == 66.75f);

			// String
			oLeft = "Hello";
			oRight = "World";
			oCombined = oLeft;
			CHECK(oCombined.Combine(oRight, false));
			CHECK(oCombined.IsString());
			CHECK(oCombined.IsString() && strcmp(oCombined.ToString(), "HelloWorld") == 0);

			// Array
			oLeft.InitType(JsonStthm::JsonValue::E_TYPE_ARRAY);
			oRight.InitType(JsonStthm::JsonValue::E_TYPE_ARRAY);
			oLeft.Append() = "Hello";
			oRight.Append() = "World";
			oCombined = oLeft;
			CHECK(oCombined.Combine(oRight, false));
			CHECK(oCombined.IsArray());
			CHECK(oCombined.GetMemberCount() == 2);
			CHECK(oCombined.IsArray() && oCombined[0].IsString() && strcmp(oCombined[0].ToString(), "Hello") == 0);
			CHECK(oCombined.IsArray() && oCombined[1].IsString() && strcmp(oCombined[1].ToString(), "World") == 0);

			// Object (replace members)
			oLeft.InitType(JsonStthm::JsonValue::E_TYPE_OBJECT);
			oRight.InitType(JsonStthm::JsonValue::E_TYPE_OBJECT);
			oLeft["Member"]["First"] = "Hello";
			oRight["Member"]["Second"] = "World";
			oCombined = oLeft;
			CHECK(oCombined.Combine(oRight, false));
			CHECK(oCombined.GetMemberCount() == 1 && oCombined["Member"].GetMemberCount() == 1);
			CHECK(oCombined["Member"]["First"].IsNull());
			CHECK(oCombined["Member"]["Second"].IsString() && strcmp(oCombined["Member"]["Second"].ToString(), "World") == 0);

			// Object (merge sub members)
			oCombined = oLeft;
			CHECK(oCombined.Combine(oRight, true));
			CHECK(oCombined.GetMemberCount() == 1 && oCombined["Member"].GetMemberCount() == 2);
			CHECK(oCombined["Member"]["First"].IsString() && strcmp(oCombined["Member"]["First"].ToString(), "Hello") == 0);
			CHECK(oCombined["Member"]["Second"].IsString() && strcmp(oCombined["Member"]["Second"].ToString(), "World") == 0);
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

		BEGIN_TEST_SUITE("JsonValue Iterator invalid")
		{
			// On empty/invalid
			{
				JsonStthm::JsonValue::Iterator it = &JsonStthm::JsonValue::INVALID;
				CHECK(it.IsValid() == false);
			}

			// On NULL
			{
				JsonStthm::JsonValue::Iterator it = NULL;
				CHECK(it.IsValid() == false);
			}
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE("JsonValue Iterator object")
		{
			JsonStthm::JsonValue oValue;

			oValue["a"].SetInteger(1);
			oValue["b"].SetFloat(2.f);

			JsonStthm::JsonValue::Iterator it = &oValue;

			CHECK(it.IsValid());
			CHECK(strcmp(it->GetName(), "a") == 0);

			++it;
			CHECK(it.IsValid());
			CHECK(strcmp( it->GetName(), "b") == 0);

			++it;
			CHECK(it.IsValid() == false);
		}
		END_TEST_SUITE();

		BEGIN_TEST_SUITE("JsonValue Iterator array")
		{
			JsonStthm::JsonValue oValue;

			oValue.Append().SetInteger(1);
			oValue.Append().SetString("Test");

			JsonStthm::JsonValue::Iterator it = &oValue;

			CHECK(it.IsValid());
			CHECK(it->IsInteger());

			++it;
			CHECK(it.IsValid());
			CHECK(it->IsString());

			++it;
			CHECK(it.IsValid() == false);

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

#ifdef INCLUDE_TEST_GASON
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
#endif // INCLUDE_TEST_GASON

#ifdef INCLUDE_TEST_SHEREDOM
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
#endif // INCLUDE_TEST_SHEREDOM

#ifdef INCLUDE_TEST_HJIANG
		BEGIN_BENCHMARK_VERSUS_CHALLENGER("hjiang/JsonXX")
			std::filebuf oFileBuf;
			CHECK_FATAL(oFileBuf.open(VERSUS_ARG, std::ios::in) != NULL)
			std::istream oIStream(&oFileBuf);
			jsonxx::Object oJson;
			CHECK(oJson.parse(oIStream))
			oFileBuf.close();
		END_BENCHMARK_VERSUS_CHALLENGER()
#endif // INCLUDE_TEST_HJIANG

#ifdef INCLUDE_TEST_NLOHMANN
		BEGIN_BENCHMARK_VERSUS_CHALLENGER("nlohmann/json")
			std::filebuf oFileBuf;
			CHECK_FATAL(oFileBuf.open(VERSUS_ARG, std::ios::in) != NULL)
			bool bOk = true;
			std::istream oIStream(&oFileBuf);
			nlohmann::json oJson;
			CHECK_TRY(oIStream >> oJson);
			oFileBuf.close();
		END_BENCHMARK_VERSUS_CHALLENGER()
#endif // INCLUDE_TEST_NLOHMANN

#ifdef INCLUDE_TEST_VINCENTHZ
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
#endif // INCLUDE_TEST_VINCENTHZ
	END_BENCHMARK_VERSUS_ARGS()

	//system("pause");
}
