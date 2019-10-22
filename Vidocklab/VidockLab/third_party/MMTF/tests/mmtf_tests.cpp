#ifdef __EMSCRIPTEN__
#define CATCH_INTERNAL_CONFIG_NO_POSIX_SIGNALS
#define CATCH_CONFIG_RUNNER
#else
#define CATCH_CONFIG_MAIN
#endif

#include "catch.hpp"

#include <mmtf.hpp>
#include <mmtf/export_helpers.hpp>


// NOTE!!! Margin is set to 0.00001
// If we ever get more specific data this may need to be altered!
template <typename T>
bool approx_equal_vector(const T& a, const T& b, float eps = 0.00001) {
	if (a.size() != b.size()) return false;
	for (std::size_t i=0; i < a.size(); ++i) {
		if (a[i] != Approx(b[i]).margin(eps)) return false;
	}
	return true;
}


std::map<std::string, msgpack::object*>
msgpack_obj_to_map(const msgpack::object& obj) {
	std::map<std::string, msgpack::object*> data_map;
	msgpack::object_kv* current_key_value = obj.via.map.ptr;
	msgpack::object_kv* last_key_value = current_key_value + obj.via.map.size;
	for (; current_key_value != last_key_value; ++current_key_value) { 
		msgpack::object* key = &(current_key_value->key); 
		msgpack::object* value = &(current_key_value->val); 
		if (key->type == msgpack::type::STR) {        
			std::string data_map_key(key->via.str.ptr, key->via.str.size);
			data_map[data_map_key] = value;
		} else {
			throw std::runtime_error("Error: Found non-string key!");
		}
	}
	return data_map;
}

TEST_CASE("assignment operator") {
	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	SECTION("basic assignment operator") {
		mmtf::StructureData sd2;
		sd2 = sd;
		REQUIRE(sd2 == sd);
	}
	SECTION("deep assignment operator") {
		mmtf::StructureData sd2;
		std::vector<int32_t> clist;
		for (int32_t i = 0; i < 256; ++i) {
			clist.push_back(i);
		}
		sd.atomProperties["256l"] = msgpack::object(clist, sd.msgpack_zone);
		sd2 = sd;
		REQUIRE(sd2 == sd);
		REQUIRE(sd2.atomProperties["256l"] == sd.atomProperties["256l"]);
	}
	SECTION("deep assignment operator") {
		mmtf::StructureData sd2;
		std::vector<int32_t> clist;
		for (int32_t i = 0; i < 256; ++i) {
			clist.push_back(i);
		}
		sd.atomProperties["256l"] = msgpack::object(clist, sd.msgpack_zone);
		sd2 = sd;
		clist.push_back(22);
		sd.atomProperties["256l"] = msgpack::object(clist, sd.msgpack_zone);
		REQUIRE(sd2 != sd);
		REQUIRE(sd2.atomProperties["256l"] != sd.atomProperties["256l"]);
	}
}


TEST_CASE("copy constructor") {
	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	SECTION("Basic copy constructor") {
		mmtf::StructureData sd2(sd);
		REQUIRE(sd2 == sd);
	}
	SECTION("Check msgpack::object copying") {
		std::vector<int32_t> clist;
		for (int32_t i = 0; i < 256; ++i) {
			clist.push_back(i);
		}
		sd.atomProperties["256l"] = msgpack::object(clist, sd.msgpack_zone);
		mmtf::StructureData sd2(sd);
		REQUIRE(sd2.atomProperties["256l"] == sd.atomProperties["256l"]);
	}
}

// Tests for structure_data.hpp
TEST_CASE("Test round trip StructureData working") {
	std::vector<std::string> works;
	works.push_back("../mmtf_spec/test-suite/mmtf/173D.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1AA6.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1AUY.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1BNA.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1CAG.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1HTQ.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1IGT.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1L2Q.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1LPV.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1MSH.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1O2F.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1R9V.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/1SKM.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/3NJW.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/3ZYB.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4CK4.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4CUP.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4OPJ.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4P3R.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4V5A.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/4Y60.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/5EMG.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/5ESW.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/empty-all0.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/empty-numChains1.mmtf");
	works.push_back("../mmtf_spec/test-suite/mmtf/empty-numModels1.mmtf");
	works.push_back("../temporary_test_data/all_canoncial.mmtf");
	works.push_back("../temporary_test_data/1PEF_with_resonance.mmtf");
	for (size_t i=0; i<works.size(); ++i) {
		mmtf::StructureData sd1, sd2;
		mmtf::decodeFromFile(sd1, works[i]);
		mmtf::encodeToFile(sd1, "test_mmtf.mmtf");
		mmtf::decodeFromFile(sd2, "test_mmtf.mmtf");
		REQUIRE( sd1 == sd2 );
	}
}


TEST_CASE("Test round trip StructureData not working - EncodeError") {
	std::string basic = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, basic);
	SECTION("Alter xCoordList") {
		sd.xCoordList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter yCoordList") {
		sd.yCoordList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter zCoordList") {
		sd.zCoordList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter bFactorList") {
		sd.bFactorList.push_back(0.334f);
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter numAtoms") {
		sd.numAtoms = 20;
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter chainIdList") {
		sd.chainIdList.push_back("xsz");
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("chainIdList one too large") {
		sd.chainIdList[0] = "IheartMMTF";
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter single groupList entry") {
		sd.groupList[0].formalChargeList.pop_back();
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
		sd.groupList[0].atomNameList.pop_back();
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
		sd.groupList[0].elementList.pop_back();
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}
	SECTION("Alter groupTypeList") {
		sd.groupTypeList[0] = 100;
		REQUIRE_THROWS_AS(mmtf::encodeToFile(sd, "test_mmtf.mmtf"), mmtf::EncodeError);
	}

}


TEST_CASE("Test StructureData DecodeError") {
	std::vector<std::string> doesnt_work;
	doesnt_work.push_back("../mmtf_spec/test-suite/mmtf/empty-mmtfVersion99999999.mmtf");
	for (size_t i=0; i<doesnt_work.size(); ++i) {
		mmtf::StructureData sd;
		REQUIRE_THROWS_AS(mmtf::decodeFromFile(sd, doesnt_work[i]), mmtf::DecodeError);
	}
}


TEST_CASE("Test DeltaRecursiveFloat enc/dec") {
	std::vector<char> encoded_data;
	// h1
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x0a);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);
	encoded_data.push_back(0xe8);
	// data
	encoded_data.push_back(0x7f);
	encoded_data.push_back(0xff);
	encoded_data.push_back(0x44);
	encoded_data.push_back(0xab);
	encoded_data.push_back(0x01);
	encoded_data.push_back(0x8f);
	encoded_data.push_back(0xff);
	encoded_data.push_back(0xca);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<float> decoded_input;
	bd.decode(decoded_input);

	std::vector<float> decoded_data;
	decoded_data.push_back(50.346f);
	decoded_data.push_back(50.745f);
	decoded_data.push_back(50.691f);

	std::vector<char> encoded_output = mmtf::encodeDeltaRecursiveFloat(decoded_data, 1000);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(approx_equal_vector(decoded_data, decoded_input));
}


TEST_CASE("Test RunLengthFloat enc/dec") {
	std::vector<char> encoded_data;
	// h1
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x09);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x64);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x64);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x03);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<float> decoded_input;
	bd.decode(decoded_input);

	std::vector<float> decoded_data;
	decoded_data.push_back(1.00f);
	decoded_data.push_back(1.00f);
	decoded_data.push_back(1.00f);

	std::vector<char> encoded_output = mmtf::encodeRunLengthFloat(decoded_data, 100);

	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(approx_equal_vector(decoded_data, decoded_input));
}


TEST_CASE("Test RunLengthDeltaInt enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x08);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x07);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x01);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x07);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int32_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int32_t> decoded_data;
	decoded_data.push_back(1);
	decoded_data.push_back(2);
	decoded_data.push_back(3);
	decoded_data.push_back(4);
	decoded_data.push_back(5);
	decoded_data.push_back(6);
	decoded_data.push_back(7);

	std::vector<char> encoded_output = mmtf::encodeRunLengthDeltaInt(decoded_data);

	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test RunLengthChar enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x06);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x41);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<char> decoded_input;
	bd.decode(decoded_input);

	std::vector<char> decoded_data;
	decoded_data.push_back('A');
	decoded_data.push_back('A');
	decoded_data.push_back('A');
	decoded_data.push_back('A');

	std::vector<char> encoded_output = mmtf::encodeRunLengthChar(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test RunLengthInt8 enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x10);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int8_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int8_t> decoded_data;
	decoded_data.push_back(2);
	decoded_data.push_back(2);
	decoded_data.push_back(2);
	decoded_data.push_back(2);

	std::vector<char> encoded_output = mmtf::encodeRunLengthInt8(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test encodeStringVector enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x05);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x06);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// data
	encoded_data.push_back('B');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('C');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back('A');
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<std::string> decoded_input;
	bd.decode(decoded_input);

	std::vector<std::string> decoded_data;
	decoded_data.push_back("B");
	decoded_data.push_back("A");
	decoded_data.push_back("C");
	decoded_data.push_back("A");
	decoded_data.push_back("A");
	decoded_data.push_back("A");

	int chain_name_max_length = 4;
	std::vector<char> encoded_output = mmtf::encodeStringVector(decoded_data, chain_name_max_length);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test int 8 enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x05);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x07);
	encoded_data.push_back(0x06);
	encoded_data.push_back(0x06);
	encoded_data.push_back(0x07);
	encoded_data.push_back(0x07);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int8_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int8_t> decoded_data;
	decoded_data.push_back(7);
	decoded_data.push_back(6);
	decoded_data.push_back(6);
	decoded_data.push_back(7);
	decoded_data.push_back(7);

	std::vector<char> encoded_output = mmtf::encodeInt8ToByte(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}


TEST_CASE("Test FourByteInt enc/dec") {
	std::vector<char> encoded_data;
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// h2
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x04);
	// h3
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	// data
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x01);

	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x01);

	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);

	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x00);
	encoded_data.push_back(0x02);

	msgpack::zone m_zone;
	msgpack::object msgp_obj(encoded_data, m_zone);

	mmtf::BinaryDecoder bd(msgp_obj, "a_test");
	std::vector<int32_t> decoded_input;
	bd.decode(decoded_input);

	std::vector<int32_t> decoded_data;
	decoded_data.push_back(1);
	decoded_data.push_back(131073);
	decoded_data.push_back(0);
	decoded_data.push_back(2);

	std::vector<char> encoded_output = mmtf::encodeFourByteInt(decoded_data);
	REQUIRE(encoded_data == encoded_output);
	REQUIRE(decoded_data.size() ==  decoded_input.size());
	REQUIRE(decoded_data == decoded_input);
}

TEST_CASE("Test bondOrderList vs bondAtomList") {
	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	SECTION("Deleting all bondOrderLists") {
		for (auto & group : sd.groupList) {
			group.bondOrderList.clear();
		}
		sd.bondOrderList.clear();
		REQUIRE(sd.hasConsistentData());
		// we write files without those fields (tested below), can we roundtrip?
		mmtf::StructureData sd2;
		mmtf::encodeToFile(sd, "test_mmtf_nobondorder.mmtf");
		mmtf::decodeFromFile(sd2, "test_mmtf_nobondorder.mmtf");
		REQUIRE( sd == sd2 );
	}
	SECTION("altering group bondOrderLists") {
		sd.groupList[0].bondOrderList.push_back(1);
		REQUIRE_FALSE(sd.hasConsistentData());
	}
	SECTION("altering sd bondOrderLists") {
		sd.bondOrderList.push_back(1);
		REQUIRE_FALSE(sd.hasConsistentData());
	}
	SECTION("not ok to have bond orders without atom list in group") {
		sd.groupList[0].bondAtomList.clear();
		REQUIRE_FALSE(sd.hasConsistentData());
	}
	SECTION("not ok to have bond orders without atom list") {
		sd.bondAtomList.clear();
		REQUIRE_FALSE(sd.hasConsistentData());
	}
}


TEST_CASE("test valid bonds") {
	std::string working_mmtf = "../temporary_test_data/all_canoncial.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	REQUIRE(sd.hasConsistentData());

	SECTION("invalid bond numbers in group") {
		int8_t original = sd.groupList[0].bondResonanceList[0];
		sd.groupList[0].bondResonanceList[0] = -3;
		REQUIRE_FALSE(sd.hasConsistentData());
		sd.groupList[0].bondResonanceList[0] = 2;
		REQUIRE_FALSE(sd.hasConsistentData());
		sd.groupList[0].bondResonanceList[0] = original;

		original = sd.groupList[0].bondOrderList[0];
		sd.groupList[0].bondOrderList[0] = 0;
		REQUIRE_FALSE(sd.hasConsistentData());
		sd.groupList[0].bondOrderList[0] = 5;
		REQUIRE_FALSE(sd.hasConsistentData());

		sd.groupList[0].bondOrderList[0] = -1;
		sd.groupList[0].bondResonanceList[0] = -1;
		REQUIRE_FALSE(sd.hasConsistentData());
	}
	SECTION("invalid bond numbers") {
		int8_t original = sd.bondResonanceList[0];
		sd.bondResonanceList[0] = -3;
		REQUIRE_FALSE(sd.hasConsistentData());
		sd.bondResonanceList[0] = 2;
		REQUIRE_FALSE(sd.hasConsistentData());
		sd.bondResonanceList[0] = original;

		original = sd.bondOrderList[0];
		sd.bondOrderList[0] = 0;
		REQUIRE_FALSE(sd.hasConsistentData());
		sd.bondOrderList[0] = 5;
		REQUIRE_FALSE(sd.hasConsistentData());

		sd.bondOrderList[0] = -1;
		sd.bondResonanceList[0] = -1;
		REQUIRE_FALSE(sd.hasConsistentData());
	}

	SECTION("invalid bondResonanceList size in group") {
		sd.groupList[0].bondResonanceList.push_back(1);
		REQUIRE_FALSE(sd.hasConsistentData());
	}
	SECTION("invalid bondResonanceList size") {
		sd.bondResonanceList.push_back(1);
		REQUIRE_FALSE(sd.hasConsistentData());
	}

	SECTION("not ok to have bond resonances without order list in group") {
		sd.groupList[0].bondOrderList.clear();
		REQUIRE_FALSE(sd.hasConsistentData());
	}
	SECTION("not ok to have bond resonances without order list") {
		sd.bondOrderList.clear();
		REQUIRE_FALSE(sd.hasConsistentData());
	}
}


TEST_CASE("test group export optional") {
	std::string working_mmtf = "../temporary_test_data/all_canoncial.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	REQUIRE(sd.hasConsistentData());

	mmtf::GroupType & first_group(sd.groupList[0]);

	SECTION("check that optional fields exist fist") {
		msgpack::zone my_zone;
		msgpack::object obj(first_group, my_zone);
		std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
		REQUIRE(my_map.find("bondOrderList") != my_map.end());
		REQUIRE(my_map.find("bondResonanceList") != my_map.end());
		REQUIRE(my_map.find("bondAtomList") != my_map.end());
	}
	SECTION("removing group bondOrderLists") {
		msgpack::zone my_zone;
		first_group.bondOrderList = std::vector<int8_t>();
		msgpack::object obj(first_group, my_zone);
		std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
		REQUIRE(my_map.find("bondOrderList") == my_map.end());
	}
	SECTION("altering group bondResonanceLists") {
		msgpack::zone my_zone;
		first_group.bondResonanceList = std::vector<int8_t>();
		msgpack::object obj(first_group, my_zone);
		std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
		REQUIRE(my_map.find("bondResonanceList") == my_map.end());
	}
	SECTION("altering group bondAtomLists") {
		msgpack::zone my_zone;
		first_group.bondAtomList = std::vector<int32_t>();
		msgpack::object obj(first_group, my_zone);
		std::map<std::string, msgpack::object*> my_map(msgpack_obj_to_map(obj));
		REQUIRE(my_map.find("bondAtomList") == my_map.end());
	}
}

TEST_CASE("atomProperties field") {
	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
	mmtf::StructureData sd, sd2, sd3;
	mmtf::decodeFromFile(sd, working_mmtf);

	/// Pack
	// 1. Make your input data
	std::vector<int32_t> clist_out;
	std::vector<int32_t> clist_in, clist_in_decoded, nothing;
	for (int32_t i = 0; i < 256; ++i) {
		clist_out.push_back(i);
	}
	// 2. msgpack zones have weird lifetimes, make sure you use the zone
	// in StructureData to avoid any weird errors
	sd.atomProperties["256_atomColorList"] = msgpack::object(clist_out, sd.msgpack_zone);
	sd.atomProperties["256_atomColorList_encoded"] = msgpack::object(mmtf::encodeRunLengthDeltaInt(clist_out), sd.msgpack_zone);

	mmtf::encodeToFile(sd, "test_atomProperties.mmtf");
	/// Done Pack

	/// Start Unpack
	mmtf::decodeFromFile(sd2, "test_atomProperties.mmtf");
	REQUIRE(sd == sd2);

	// Check round-trip after reading extra data
	// (i.e. we can safely read/write unknown extra data)
	mmtf::encodeToFile(sd2, "test_atomProperties2.mmtf");
	mmtf::decodeFromFile(sd3, "test_atomProperties2.mmtf");
	REQUIRE(sd2 == sd3);

	// Retrieve our 256 color list via convert
	mmtf::MapDecoder atomProperties_MD(sd3.atomProperties);
	atomProperties_MD.decode("256_atomColorList", true, clist_in);
	atomProperties_MD.decode("256_atomColorList_encoded", true, clist_in_decoded);

	/// Done Unpack
	REQUIRE(clist_out == clist_in);
	REQUIRE(clist_in == clist_in_decoded);

	// you would catch mmtf::DecodeError if you wanted continue even after using
	// required=true.
	REQUIRE_THROWS_AS(atomProperties_MD.decode("NONEXISTANT", true, nothing), mmtf::DecodeError);
}


TEST_CASE("Test export_helpers") {
	std::string working_mmtf = "../mmtf_spec/test-suite/mmtf/3NJW.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);
	size_t numbonds_ref = sd.numBonds;
	std::vector<int32_t> bonddata;

	// test requirement: groupTypeList has duplicates
	REQUIRE(sd.groupTypeList.size() != sd.groupList.size());

	// must throw with duplicates
	REQUIRE_THROWS_AS(mmtf::BondAdder(sd), mmtf::EncodeError);

	// make groupTypeList non-duplicate
	for (size_t i = 1; i < sd.groupTypeList.size(); ++i) {
		for (size_t j = 0; j < i; ++j) {
			if (sd.groupTypeList[i] == sd.groupTypeList[j]) {
				sd.groupTypeList[i] = sd.groupList.size();
				sd.groupList.push_back(sd.groupList[sd.groupTypeList[j]]);
				break;
			}
		}
	}

	// test assert: groupTypeList has no duplicates anymore
	REQUIRE(sd.groupTypeList.size() == sd.groupList.size());

	// remove group bonds
	size_t offset = 0;
	for (auto groupType : sd.groupTypeList) {
		auto & group = sd.groupList[groupType];
		for (size_t i = 0; i < group.bondOrderList.size(); ++i) {
			bonddata.push_back(group.bondAtomList[i * 2] + offset);
			bonddata.push_back(group.bondAtomList[i * 2 + 1] + offset);
			bonddata.push_back(group.bondOrderList[i]);
			--sd.numBonds;
		}
		offset += group.atomNameList.size();
		group.bondAtomList.clear();
		group.bondOrderList.clear();
	}

	// remove global bonds
	for (size_t i = 0; i < sd.bondOrderList.size(); ++i) {
		bonddata.push_back(sd.bondAtomList[i * 2]);
		bonddata.push_back(sd.bondAtomList[i * 2 + 1]);
		bonddata.push_back(sd.bondOrderList[i]);
		--sd.numBonds;
	}
	sd.bondAtomList.clear();
	sd.bondOrderList.clear();

	// test assert: all bonds have been transferred to `bonddata`
	REQUIRE(bonddata.size() == numbonds_ref * 3);
	REQUIRE(sd.numBonds == 0);
	REQUIRE(sd.hasConsistentData());

	// re-add bonds
	mmtf::BondAdder bondadder(sd);
	for (size_t i = 0; i < bonddata.size(); i += 3) {
		REQUIRE(bondadder(bonddata[i], bonddata[i + 1], bonddata[i + 2]));
	}
	REQUIRE(sd.numBonds == numbonds_ref);
	REQUIRE(sd.hasConsistentData());

	// re-compress groupTypeList
	mmtf::compressGroupList(sd);
	REQUIRE(sd.groupTypeList.size() != sd.groupList.size());
	REQUIRE(sd.hasConsistentData());

	// compare with original data
	mmtf::StructureData sd_ref;
	mmtf::decodeFromFile(sd_ref, working_mmtf);
	REQUIRE(sd_ref.bondAtomList == sd.bondAtomList);
	REQUIRE(sd_ref.bondOrderList == sd.bondOrderList);
	REQUIRE(sd_ref.groupTypeList == sd.groupTypeList);
	REQUIRE(sd_ref.groupList == sd.groupList);
}


TEST_CASE("Test is_hetatm (chain_index version)") {
	std::string working_mmtf = "../temporary_test_data/3zqs.mmtf";
	mmtf::StructureData sd;
	mmtf::decodeFromFile(sd, working_mmtf);

	SECTION("CHECK CHAINS") {
		int modelIndex = 0;
		int chainIndex = 0;
		for (int i = 0; i < sd.numModels; i++, modelIndex++) {
			for (int j = 0; j < sd.chainsPerModel[modelIndex]; j++, chainIndex++) {
				// chain indices 0 and 1 belong to a polymer entity
				// all others should be marked as hetatm
				if (chainIndex < 2) {
					REQUIRE(is_polymer(chainIndex, sd.entityList));
				} else {
					REQUIRE_FALSE(is_polymer(chainIndex, sd.entityList));
				}
			}
		}
	}

	SECTION("CHECK WITH GROUP") {
		// chain indices 0 and 1 belong to polymer entity 0
		std::string expected_sequence = sd.entityList[0].sequence;
		int modelIndex = 0;
		int chainIndex = 0;
		int groupIndex = 0;
		for (int i = 0; i < sd.numModels; i++, modelIndex++) {
			for (int j = 0; j < sd.chainsPerModel[modelIndex]; j++, chainIndex++) {
				std::string found_seq = "";
				for (int k = 0; k < sd.groupsPerChain[chainIndex]; k++, groupIndex++) {
					const mmtf::GroupType& group =
						sd.groupList[sd.groupTypeList[groupIndex]];
					bool hetatm = is_hetatm(chainIndex, sd.entityList, group);
					if (chainIndex < 2) {
						if (!hetatm) found_seq += group.singleLetterCode;
					} else {
						REQUIRE(hetatm);
					}
				}
				if (chainIndex < 2) REQUIRE(expected_sequence == found_seq);
			}
		}
	}

	SECTION("throw check") {
		REQUIRE_THROWS_AS(is_polymer(999, sd.entityList), mmtf::DecodeError);
	}
}


#ifdef __EMSCRIPTEN__
#include <emscripten.h>

int main(int argc, char* argv[]) {
    // Give node.js an access to the root filesystem
    EM_ASM(
        FS.mkdir('root');
        FS.mount(NODEFS, { root: '/' }, 'root');
        FS.chdir('root/' + process.cwd() + '/../');
    );

    return Catch::Session().run(argc, argv);
}
#endif

// vi:noexpandtab:sw=4:ts=4
