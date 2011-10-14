/* -*- mia-c++  -*-
 *
 * Copyright (c) Leipzig, Madrid 1999-2011 Gert Wollny
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif

#define BOOST_TEST_MAIN
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <stdexcept>
#include <climits>

#include <mia/core/cmdlineparser.hh>
#include <mia/core/msgstream.hh>

NS_MIA_USE
using namespace std;
using namespace boost::unit_test;

struct CmdlineParserFixture {
	CmdlineParserFixture():m_level(cverb.get_level()) {
	}
	~CmdlineParserFixture() {
		cverb.set_verbosity(m_level);
	}
private:
	vstream::Level m_level;

};

const SProgramDescrption general_help = {
	"Test", 
	"This program tests the command line parser.", 
	NULL, 
	NULL, 
	"something"
}; 


BOOST_FIXTURE_TEST_CASE( test_set_option, CmdlineParserFixture )
{
	string value ="false";
	set<string> possible_values;
	possible_values.insert("uno");
	possible_values.insert("dos");
	possible_values.insert("tres");

	PCmdOption popt(make_opt(value, possible_values, "string", 's', "a string option"));

	try {
		popt->set_value("uno");
		BOOST_CHECK(value == "uno");
		BOOST_CHECK(popt->get_value_as_string() == "uno");
	}
	catch (invalid_argument& x) {
		BOOST_FAIL(x.what());
	}

	try {
		const char *str_value_err = "cuadro";
		popt->set_value(str_value_err);
		BOOST_FAIL("error value not detected");
	}
	catch (invalid_argument& x) {
		BOOST_MESSAGE(string("Caught:") + string(x.what()));
	}
}

BOOST_FIXTURE_TEST_CASE( test_string_vector_option, CmdlineParserFixture )
{
	const char *str_value = "string,list";
	vector<string> value;
	PCmdOption popt(make_opt(value, "string", 's', "a string option", "string"));
	try {
		popt->set_value(str_value);
		BOOST_REQUIRE(value.size() == 2);
		BOOST_CHECK_EQUAL(value[0], "string");
		BOOST_CHECK_EQUAL(value[1], "list");
		BOOST_CHECK_EQUAL(popt->get_value_as_string(), string(str_value));
	}
	catch (invalid_argument& x) {
		BOOST_FAIL(x.what());
	}
}

BOOST_FIXTURE_TEST_CASE( test_float_vector_option, CmdlineParserFixture )
{
	const char *str_value = "1.2,1.4";
	vector<float> value;
	PCmdOption popt(make_opt(value, "float", 's', "a float vector option", "float"));
	try {
		popt->set_value(str_value);
		BOOST_REQUIRE(value.size() == 2);
		BOOST_CHECK_CLOSE(value[0], 1.2f, 0.0001f);
		BOOST_CHECK_CLOSE(value[1], 1.4f, 0.0001f);
		BOOST_CHECK_EQUAL(popt->get_value_as_string(), string(str_value));
	}
	catch (invalid_argument& x) {
		BOOST_FAIL(x.what());
	}
}

BOOST_FIXTURE_TEST_CASE( test_string_option, CmdlineParserFixture )
{
	string value = "string with spaces";
	PCmdOption popt(make_opt(value, "string", 's', "a string option", "string"));

	const char *str_value = "let there be spaces";
	try {
		popt->set_value(str_value);
		BOOST_CHECK(value == string(str_value));
		BOOST_CHECK(popt->get_value_as_string() == string(str_value));
	}
	catch (invalid_argument& x) {
		BOOST_FAIL(x.what());
	}
}

BOOST_FIXTURE_TEST_CASE( test_float_option, CmdlineParserFixture )
{
	float value = 10;
	PCmdOption popt(make_opt(value, "float", 'f', "a float option", "float"));
	const char *str_value = "12.2";
	try {
		popt->set_value(str_value);
		BOOST_CHECK(value == 12.2f);
		BOOST_CHECK(popt->get_value_as_string() == "12.2");
	}
	catch (invalid_argument& x) {
		BOOST_FAIL(x.what());
	}

	const char *str_value_err = "12.1s";
	try {
		popt->set_value(str_value_err);
		BOOST_FAIL("error value not detected");
	}
	catch (invalid_argument& x) {
		BOOST_MESSAGE(string("Caught:") + string(x.what()));
	}
}


BOOST_FIXTURE_TEST_CASE( test_int_option, CmdlineParserFixture )
{
	int value = 10;
	PCmdOption popt(make_opt(value, "int", 'i', "a int option", "int"));
	const char *str_value = "12";
	try {
		popt->set_value(str_value);
		BOOST_CHECK(value == 12);

		BOOST_CHECK(popt->get_value_as_string() == "12");
	}
	catch (invalid_argument& x) {
		BOOST_FAIL(x.what());
	}

	const char *str_value_err = "12.1";
	try {
		popt->set_value(str_value_err);
		BOOST_FAIL("error value not detected");
	}
	catch (invalid_argument& x) {
		BOOST_MESSAGE(string("Caught:") + string(x.what()));
	}
}

enum ETestEnums {te_one, te_two, te_three, te_undefined};

BOOST_FIXTURE_TEST_CASE( test_dict_option, CmdlineParserFixture )
{

	TDictMap<ETestEnums>::Table table[] =
		{
			{"one", te_one, "test 1"},
			{"two", te_two, "test 2"},
			{"three", te_three, "test 3"},
			{NULL, te_undefined, ""}
		};
	TDictMap<ETestEnums> map(table);

	ETestEnums value = te_undefined;

	CCmdOptionList olist(general_help);

	olist.add(make_opt(value, map, "dict", 'd', "a dictmap option", "dict"));
	vector<const char *> options;

	options.push_back("self");
	options.push_back("-dtwo");
	BOOST_CHECK_EQUAL(olist.parse(options.size(), &options[0], false), CCmdOptionList::hr_no); 

	BOOST_CHECK( olist.get_remaining().size() == 0);
	BOOST_CHECK(value == te_two);
}

BOOST_FIXTURE_TEST_CASE( test_flagstring_option, CmdlineParserFixture )
{

	CFlagString::Table table[] =	{
		{'o', te_one},
		{'t', te_two},
		{'h', te_three},
		{0, te_undefined}
	};
	CFlagString map(table);

	int value = te_undefined;

	CCmdOptionList olist(general_help);

	olist.add(make_opt(value, map, "flags", 'f', "a flagstring option", "flags"));
	vector<const char *> options;

	options.push_back("self");
	options.push_back("-f");
	options.push_back("ot");
	BOOST_CHECK_EQUAL(olist.parse(options.size(), &options[0], false), CCmdOptionList::hr_no); 

	BOOST_CHECK( olist.get_remaining().size() == 0);
	BOOST_CHECK(value == te_two || te_one);
}


BOOST_FIXTURE_TEST_CASE( test_parser, CmdlineParserFixture )
{
	vector<const char *> options;

	options.push_back("self");
	options.push_back("leftover");

	int int_value1 = 10;
	int int_value2 = 10;
	int int_value3 = 10;
	unsigned short usval = 12;
	float float_value = 1.2f;
	string s("string");
	vector<string> vector_value;

	bool bingo = false;
	bool bingo2 = false;

	options.push_back("-i12");
	options.push_back("-k");
	options.push_back("13");
	options.push_back("--int-3");
	options.push_back("21");
	options.push_back("-f1.3");
	options.push_back("--string");
	options.push_back("string");
	options.push_back("--vector-string");
	options.push_back("string1,string2,string3");
	options.push_back("-b2u16");
	options.push_back("leftover2");


	CCmdOptionList olist(general_help);

	olist.add(make_opt(int_value1,  "int1", 'i',"a int option", "int1"));
	olist.add(make_opt(int_value2, "int2",  'k', "another int option", "int2"));
	olist.add(make_opt(int_value3,  "int-3", 0, "yet another int option", "int3"));
	olist.add(make_opt(float_value, "float", 'f', "a float option", "float"));
	olist.add(make_opt(s,  "string", 's',"a string option", "string"));
	olist.add(make_opt(bingo,  "bingo", 'b', "a bool option", "bool"));
	olist.add(make_opt(bingo2,  "bingo2", '2', "another bool option", "bool"));
	olist.add(make_opt(usval,  "ushort", 'u', "a short int option", "ushort"));
	olist.add(make_opt(vector_value,  "vector-string", 'S', "a vector of strings", "vstring"));

	BOOST_CHECK_EQUAL(olist.parse(options.size(), &options[0], true),  CCmdOptionList::hr_no);

	for( vector<const char *>::const_iterator i = olist.get_remaining().begin(); i != olist.get_remaining().end(); ++i)
		BOOST_MESSAGE(*i);

	BOOST_CHECK_EQUAL(int_value1,12);
	BOOST_CHECK_EQUAL(int_value2,13);
	BOOST_CHECK_EQUAL(int_value3, 21);
	BOOST_CHECK_EQUAL(float_value, 1.3f);
	BOOST_CHECK_EQUAL(usval, 16);
	BOOST_CHECK_EQUAL(s, string("string"));

	BOOST_REQUIRE(vector_value.size() == 3);
	BOOST_CHECK_EQUAL(vector_value[0], string("string1"));
	BOOST_CHECK_EQUAL(vector_value[1], string("string2"));
	BOOST_CHECK_EQUAL(vector_value[2], string("string3"));

	BOOST_CHECK_EQUAL(olist.get_remaining().size(), 2u);
}

BOOST_FIXTURE_TEST_CASE( test_parser_errors1, CmdlineParserFixture )
{
	vector<const char *> options;
	options.push_back("self1");
	options.push_back("-H2u16");
	bool bool_value = false;
	bool dummy; 

	CCmdOptionList olist(general_help);
	olist.add(make_opt(bool_value, "bool", 'H', "a bool option", "bool"));

	BOOST_CHECK_THROW(dummy = (olist.parse(options.size(), &options[0], false) == CCmdOptionList::hr_no), invalid_argument); 
}


BOOST_FIXTURE_TEST_CASE( test_parser_errors2, CmdlineParserFixture )
{
	vector<const char *> options;
	options.push_back("self1");
	options.push_back("-H2u16");
	bool bool_value = false;
	bool dummy; 
	CCmdOptionList olist(general_help);
	olist.add(make_opt(bool_value, "bool", 'H', "a bool option", "bool"));

	BOOST_CHECK_THROW(dummy = (olist.parse(options.size(), &options[0], false) == CCmdOptionList::hr_no), invalid_argument); 
}

NS_MIA_USE; 
int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
#ifdef WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif
	return ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}

