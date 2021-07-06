#include <cinttypes>
#include <ciso646>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include "simdjson.h"
using namespace simdjson;

#include "test_ondemand.h"
namespace tostring_tests {
const char *test_files[] = {
    TWITTER_JSON, TWITTER_TIMELINE_JSON, REPEAT_JSON, CANADA_JSON,
    MESH_JSON,    APACHE_JSON,           GSOC_JSON};

#if SIMDJSON_EXCEPTIONS
bool issue1607() {
  TEST_START();
  auto cars_json = R"( { "test": "result"  }  )"_padded;
  ondemand::parser parser;
  ondemand::document doc = parser.iterate(cars_json);
  std::string_view expected = R"("result")";
  std::string_view result = simdjson::to_json_string(doc["test"]);
  std::cout << "'"<< result << "'" << std::endl;
  ASSERT_EQUAL(result, expected);
  TEST_SUCCEED();
}

bool minify_demo() {
  TEST_START();
  ondemand::parser parser;
  auto cars_json = R"( { "test": "result"  }  )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(cars_json).get(doc) );
  std::cout << simdjson::to_json_string(doc["test"]) << std::endl;
  TEST_SUCCEED();
}

bool minify_demo2() {
  TEST_START();
  ondemand::parser parser;
  auto cars_json = R"( { "test": "result"  }  )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(cars_json).get(doc) );
  std::cout << std::string_view(doc["test"]) << std::endl;
  TEST_SUCCEED();
}

/**
 * The general idea of these tests if that if you take a JSON file,
 * load it, then convert it into a string, then parse that, and
 * convert it again into a second string, then the two strings should
 * be  identifical. If not, then something was lost or added in the
 * process.
 */

bool load_to_string(const char *filename) {
  simdjson::ondemand::parser parser;
  std::cout << "Loading " << filename << std::endl;
  simdjson::padded_string docdata;
  auto error = simdjson::padded_string::load(filename).get(docdata);
  if (error) {
    std::cerr << "could not load " << filename << " got " << error << std::endl;
    return false;
  }
  std::cout << "file loaded: " << docdata.size() << " bytes." << std::endl;
  simdjson::ondemand::document doc;
    auto cars_json = R"( { "test": "result"  }  )"_padded;

  error = parser.iterate(cars_json).get(doc);
  if (error) {
    std::cerr << error << std::endl;
    return false;
  }
  std::cout << "serializing once." << std::endl;
  std::string_view serial1 = simdjson::to_json_string(doc);
  error = parser.iterate(serial1, serial1.size() + simdjson::SIMDJSON_PADDING).get(doc);
  if (error) {
    std::cerr << error << std::endl;
    return false;
  }
  std::cout << "serializing twice." << std::endl;
  std::string_view serial2 = simdjson::to_json_string(doc);
  bool match = (serial1 == serial2);
  if (match) {
    std::cout << "Parsing to_string and calling to_string again results in the "
                 "same content. "
              << "Got " << serial1.size() << " bytes." << std::endl;
  }
  return match;
}

bool minify_test() {
  TEST_START();
  for (size_t i = 0; i < sizeof(test_files) / sizeof(test_files[0]); i++) {
    bool ok = load_to_string(test_files[i]);
    if (!ok) {
      return false;
    }
  }
  TEST_SUCCEED();
}

#endif // SIMDJSON_EXCEPTIONS

bool load_to_string_exceptionless(const char *filename) {
  simdjson::ondemand::parser parser;
  std::cout << "Loading " << filename << std::endl;
  simdjson::padded_string docdata;
  auto error = simdjson::padded_string::load(filename).get(docdata);
  if (error) {
    std::cerr << "could not load " << filename << " got " << error << std::endl;
    return false;
  }
  std::cout << "file loaded: " << docdata.size() << " bytes." << std::endl;
  simdjson::ondemand::document doc;
  error = parser.iterate(docdata).get(doc);
  if (error) {
    std::cerr << error << std::endl;
    return false;
  }
  std::cout << "serializing once." << std::endl;
  std::string_view serial1;
  error = simdjson::to_json_string(doc).get(serial1);
  if (error) {
    std::cerr << error << std::endl;
    return false;
  }
  error = parser.iterate(serial1, serial1.size() + simdjson::SIMDJSON_PADDING).get(doc);
  if (error) {
    std::cerr << error << std::endl;
    return false;
  }
  std::cout << "serializing twice." << std::endl;
  std::string_view serial2;
  error = simdjson::to_json_string(doc).get(serial2);
  if (error) {
    std::cerr << error << std::endl;
    return false;
  }
  bool match = (serial1 == serial2);
  if (match) {
    std::cout << "Parsing to_string and calling to_string again results in the "
                 "same content. "
              << "Got " << serial1.size() << " bytes." << std::endl;
  }
  return match;
}

bool minify_exceptionless_test() {
  TEST_START();
  for (size_t i = 0; i < sizeof(test_files) / sizeof(test_files[0]); i++) {
    bool ok = load_to_string_exceptionless(test_files[i]);
    if (!ok) {
      return false;
    }
  }
  return true;
}

bool at_start_array() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( [111,2,3,5] )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::array array;
  ASSERT_SUCCESS( doc.get_array().get(array) );
  std::string_view serial;
  ASSERT_SUCCESS( simdjson::to_json_string(array).get(serial));
  ASSERT_EQUAL(serial, "[111,2,3,5]");
  TEST_SUCCEED();
}


bool at_start_object() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( {"a":1, "b":2, "c": 3 } )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::object object;
  ASSERT_SUCCESS( doc.get_object().get(object) );
  std::string_view serial;
  ASSERT_SUCCESS( simdjson::to_json_string(object).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  ASSERT_SUCCESS( simdjson::to_json_string(object).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  TEST_SUCCEED();
}

bool in_middle_array() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( [111,{"a":1},3,5] )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::array array;
  ASSERT_SUCCESS( doc.get_array().get(array) );
  auto i = array.begin();
  int64_t x;
  ASSERT_SUCCESS( (*i).get_int64().get(x) );
  ASSERT_EQUAL(x, 111);
  std::string_view serial;
  ASSERT_SUCCESS( simdjson::to_json_string(array).get(serial));
  ASSERT_EQUAL(serial, "[111,{\"a\":1},3,5]");
  ASSERT_SUCCESS( simdjson::to_json_string(doc).get(serial));
  ASSERT_EQUAL(serial, "[111,{\"a\":1},3,5]");
  TEST_SUCCEED();
}

bool at_middle_object() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( {"a":1, "b":2, "c": 3 } )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::object object;
  ASSERT_SUCCESS( doc.get_object().get(object) );
  int64_t x;
  ASSERT_SUCCESS(object["b"].get_int64().get(x));
  ASSERT_EQUAL(x,2);
  std::string_view serial;
  ASSERT_SUCCESS( simdjson::to_json_string(object).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  ASSERT_SUCCESS( simdjson::to_json_string(doc).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  TEST_SUCCEED();
}

bool at_middle_object_just_key() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( {"a":1, "b":2, "c": 3 } )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::object object;
  ASSERT_SUCCESS( doc.get_object().get(object) );
  ondemand::value x;
  ASSERT_SUCCESS(object["b"].get(x));
  std::string_view serial;
  ASSERT_SUCCESS( simdjson::to_json_string(object).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  ASSERT_SUCCESS( simdjson::to_json_string(doc).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  TEST_SUCCEED();
}


bool at_end_object() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( {"a":1, "b":2, "c": 3 } )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::object object;
  ASSERT_SUCCESS( doc.get_object().get(object) );
  int64_t x;
  ASSERT_ERROR(object["bcc"].get_int64().get(x), NO_SUCH_FIELD);
  std::string_view serial;
  ASSERT_SUCCESS( simdjson::to_json_string(object).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  ASSERT_SUCCESS( simdjson::to_json_string(doc).get(serial));
  ASSERT_EQUAL(serial, R"({"a":1, "b":2, "c": 3 })");
  TEST_SUCCEED();
}

bool at_array_end() {
  TEST_START();
  ondemand::parser parser;
  std::string_view serial;

  auto arr_json = R"( [111,2,3,5] )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::array array;
  ASSERT_SUCCESS( doc.get_array().get(array) );
  auto i = array.begin();
  int64_t x;
  ASSERT_SUCCESS( (*i).get_int64().get(x) );
  ASSERT_EQUAL(x, 111);
  ++i;
  ASSERT_SUCCESS( (*i).get_int64().get(x) );
  ASSERT_EQUAL(x, 2);
  ++i;
  ASSERT_SUCCESS( (*i).get_int64().get(x) );
  ASSERT_EQUAL(x, 3);
  ++i;
  ASSERT_SUCCESS( (*i).get_int64().get(x) );
  ASSERT_EQUAL(x, 5);
  ASSERT_SUCCESS( simdjson::to_json_string(array).get(serial));
  ASSERT_EQUAL(serial, "[111,2,3,5]");
  ASSERT_SUCCESS( simdjson::to_json_string(doc).get(serial));
  ASSERT_EQUAL(serial, "[111,2,3,5]");
  TEST_SUCCEED();
}

bool complex_case() {
  TEST_START();
  ondemand::parser parser;
  auto arr_json = R"( {"array":[1,2,3], "objects":[{"id":1}, {"id":2}, {"id":3}]} )"_padded;
  ondemand::document doc;
  ASSERT_SUCCESS( parser.iterate(arr_json).get(doc) );
  ondemand::object obj;
  ASSERT_SUCCESS( doc.get_object().get(obj) );
  ondemand::array array;
  ASSERT_SUCCESS( obj["objects"].get_array().get(array) );
  std::string_view serial;
  for(auto v : array) {
    ondemand::object object;
    ASSERT_SUCCESS( v.get_object().get(object) );
    int64_t x;
    ASSERT_SUCCESS(object["id"].get_int64().get(x));
    if(x / 2 * 2 != x) {
      ASSERT_SUCCESS( simdjson::to_json_string(object).get(serial));
      ASSERT_EQUAL(serial, "{\"id\":"+std::to_string(x)+"}");
    }
  }
  ASSERT_SUCCESS( simdjson::to_json_string(array).get(serial));
  ASSERT_EQUAL(serial, R"([{"id":1}, {"id":2}, {"id":3}])");
  ASSERT_SUCCESS( simdjson::to_json_string(obj).get(serial));
  ASSERT_EQUAL(serial, R"({"array":[1,2,3], "objects":[{"id":1}, {"id":2}, {"id":3}]})");
  ASSERT_SUCCESS( simdjson::to_json_string(doc).get(serial));
  ASSERT_EQUAL(serial, R"({"array":[1,2,3], "objects":[{"id":1}, {"id":2}, {"id":3}]})");
  TEST_SUCCEED();
}

bool run() {
  return
      complex_case() &&
      at_start_object() &&
      at_middle_object() &&
      at_middle_object_just_key() &&
      at_end_object() &&
      at_start_array() &&
      in_middle_array() &&
      at_array_end() &&
#if SIMDJSON_EXCEPTIONS
      issue1607() &&
      minify_demo() &&
      minify_demo2() &&
      minify_test() &&
#endif // SIMDJSON_EXCEPTIONS
      minify_exceptionless_test() &&
      true;
}

} // namespace tostring_tests

int main(int argc, char *argv[]) {
  return test_main(argc, argv, tostring_tests::run);
}
