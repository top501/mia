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


#include <config.h>
#include <miaconfig.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <map>
#include <ctype.h>
#include <stdexcept>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_LIBXMLPP
#include <libxml++/libxml++.h>
#endif

#include <mia/core/tools.hh>
#include <mia/core/msgstream.hh>
#include <mia/core/cmdlineparser.hh>
#include <mia/core/fixedwidthoutput.hh>

extern void print_full_copyright(const char *name);


NS_MIA_BEGIN
using namespace std;

CCmdOption::CCmdOption(char short_opt, const char *long_opt, 
		       const char *long_help, const char *short_help, Flags flags):
	m_short_opt(short_opt), 
	m_long_opt(long_opt),
	m_long_help(long_help), 
	m_short_help(short_help),
	m_flags(flags)
{
        assert(long_opt);
        assert(long_help);
}

CCmdOption::~CCmdOption()
{
}

size_t CCmdOption::get_needed_args() const
{
	return do_get_needed_args();
}


void CCmdOption::add_option(CShortoptionMap& sm, CLongoptionMap& lm)
{
	do_add_option(sm, lm);
}

void CCmdOption::print_short_help(std::ostream& os) const
{
	do_print_short_help(os);
}

void CCmdOption::get_long_help(std::ostream& os) const
{
	os << long_help();
	do_get_long_help(os);
}

const char *CCmdOption::long_help() const
{
	return m_long_help;
}

char CCmdOption::get_short_option() const
{
	return m_short_opt; 
}

const char *CCmdOption::get_long_option() const
{
	return m_long_opt;
}

void CCmdOption::clear_required()
{
	m_flags = static_cast<CCmdOption::Flags>(m_flags & (~required)); 
}

bool CCmdOption::is_required() const
{
	return (m_flags & required ) == required; 
}

void CCmdOption::get_opt_help(std::ostream& os) const
{
	do_get_opt_help(os);
}

void   CCmdOption::write_value(std::ostream& os) const
{
	do_write_value(os);
}

const std::string CCmdOption::get_value_as_string() const
{
	return do_get_value_as_string();
}

const std::string CCmdOption::do_get_value_as_string() const
{
	return "";
}

size_t CCmdOption::do_get_needed_args() const
{
	return 1; 
}

#if 0 
void CCmdOption::do_get_long_help(std::ostream& /*os*/) const
{
}
#endif 

void CCmdOption::do_print_short_help(std::ostream& os) const
{
	if ( m_short_opt ) {
		os << '-' << m_short_opt; 
		if (  m_short_help )
			os << " " << m_short_help;
		os  << " ";
	}
}

void CCmdOption::do_get_opt_help(std::ostream& os) const
{
	if ( get_short_option() )
		os  << "  -" << get_short_option() << " ";
	else
		os  << "     ";
	os << "--" << get_long_option();
	write_value(os);
	os << " ";
}

void CCmdOption::set_value(const char *str_value)
{
	bool result;

	try {
		result = do_set_value(str_value);
	}

	catch (std::invalid_argument& x) {
		stringstream msg;
		msg << "Error parsing --" << get_long_option() << ":" << x.what();
		throw std::invalid_argument(msg.str());
	}

	if (!result) {
		throw invalid_argument(string("Error parsing option '") +
				       string(get_long_option()) +
				       string("' from input '") +
				       string(str_value) + string("'"));
	}
	clear_required();
}

void CCmdOption::do_add_option(CShortoptionMap& sm, CLongoptionMap& lm)
{
	TRACE_FUNCTION;
	if (get_short_option() != 0) {
		assert(sm.find(get_short_option()) == sm.end() &&
		       "don't add the same short option twice" );
		sm[get_short_option()] = this;
	}

	if (get_long_option() != 0) {
		assert(lm.find(get_long_option()) == lm.end() &&
		       "don't add the same long option twice");
		lm[get_long_option()] = this;
	}
}

struct CCmdOptionListData {
	CShortoptionMap short_map;
	CLongoptionMap  long_map;
	typedef vector<PCmdOption> COptionsGroup;
	typedef map<string, COptionsGroup > COptionsMap;
	COptionsMap options;
	COptionsMap::iterator current_group;
	vector<const char *> remaining;

	bool help;
	bool help_xml; 
	bool usage;
	bool copyright;
	vstream::Level verbose;

	CCmdOptionListData(const SProgramDescrption& description); 
	CCmdOptionListData(const string& general_help);

	void add(PCmdOption opt);
	void add(const string& group, PCmdOption opt);
	
	CHistoryRecord get_values() const;

	void set_current_group(const string& name);

	CCmdOption *find_option(const char *key) const;
	CCmdOption *find_option(char key) const;

#ifdef HAVE_LIBXMLPP
	void print_help_xml(const char *progname, bool has_additional) const; 
#endif
	void print_help(const char *name_help, bool has_additional) const;
	void print_usage(const char *name_help) const;

	vector<const char *> has_unset_required_options() const; 
	string m_general_help; 
	string m_program_group;  
	string m_program_example_descr;
	string m_program_example_code; 
	string m_free_parametertype; 
};

CCmdSetOption::CCmdSetOption(std::string& val, const std::set<std::string>& set, 
			     char short_opt, const char *long_opt, const char *long_help,
			     const char *short_help, Flags flags):
	CCmdOption(short_opt, long_opt, long_help, short_help,  flags),
	m_value(val),
	m_set(set)
{
}


bool CCmdSetOption::do_set_value(const char *str_value)
{
	if (m_set.find(str_value) == m_set.end())
		return false;
	m_value = str_value;
	return true;
}

size_t CCmdSetOption::do_get_needed_args() const
{
	return 1;
}

void CCmdSetOption::do_write_value(std::ostream& os) const
{
	if (m_value.empty()) 
		if (required)
			os << "[required] "; 
		else
			os << "=NULL ";
	else 
		os << "=" << m_value;
}

void CCmdSetOption::do_get_long_help(std::ostream& os) const
{
	if (m_set.size() > 0) {
		os << "\n(" ;
		std::set<std::string>::const_iterator i = m_set.begin();
		os << *i;
		++i;
		while ( i != m_set.end() )
			os << '|' << *i++;
		os  << ")";
	}
}

const std::string CCmdSetOption::do_get_value_as_string() const
{
	return m_value;
}

const char *g_help_optiongroup="Help & Info"; 
const char *g_basic_copyright = 
	"This software is copyright (c) Gert Wollny et al. "
	"It comes with  ABSOLUTELY NO WARRANTY and you may redistribute it "
	"under the terms of the GNU GENERAL PUBLIC LICENSE Version 3 (or later). "
	"For more information run the program with the option '--copyright'.\n"; 

CCmdOptionListData::CCmdOptionListData(const SProgramDescrption& description):
	help(false),
	help_xml(false), 
	usage(false),
	copyright(false),
	verbose(vstream::ml_warning), 
	m_general_help(description.description), 
	m_program_group(description.group), 
	m_program_example_descr(description.example_descr?description.example_descr:"" ),
	m_program_example_code(description.example_code?description.example_code:"")
{
	options[""] = vector<PCmdOption>();

	set_current_group(g_help_optiongroup);
	add(make_opt(verbose, g_verbose_dict, "verbose",  'V', 
		     "verbosity of output, print messages of given level and higher priorities. Supported priorities starting at lowest level are:",
		     CCmdOption::not_required));
	add(make_opt(copyright,  "copyright", 0, "print copyright information", 
		     CCmdOption::not_required));
	add(make_opt(help,  "help", 'h', "print this help", 
		     CCmdOption::not_required));
#ifdef HAVE_LIBXMLPP
	add(make_opt(help_xml,  "help-xml", 0, "print help formatted as XML", 
		     CCmdOption::not_required));
#endif
	add(make_opt(usage,  "usage", '?', "print a short help", 
		     CCmdOption::not_required));
	set_current_group("");
}

CCmdOptionListData::CCmdOptionListData(const string& general_help):
	help(false),
	help_xml(false), 
	usage(false),
	copyright(false),
	verbose(vstream::ml_warning), 
	m_general_help(general_help), 
	m_program_group("no group"), 
	m_program_example_descr(NULL),
	m_program_example_code(NULL)
{
	options[""] = vector<PCmdOption>();

	set_current_group(g_help_optiongroup);
	add(make_opt(verbose, g_verbose_dict, "verbose",  'V', "verbosity of output", 
		     CCmdOption::not_required));
	add(make_opt(copyright,  "copyright", 0, "print copyright information", 
		     CCmdOption::not_required));
	add(make_opt(help,  "help", 'h', "print this help", 
		     CCmdOption::not_required));
#ifdef HAVE_LIBXMLPP
	add(make_opt(help_xml,  "help-xml", 0, "print help formatted as XML", 
		     CCmdOption::not_required));
#endif
	add(make_opt(usage,  "usage", '?', "print a short help", 
		     CCmdOption::not_required));
	set_current_group("");
}

void CCmdOptionListData::add(PCmdOption opt)
{
	opt->add_option(short_map, long_map);
	current_group->second.push_back(opt);
}

CCmdOption *CCmdOptionListData::find_option(const char *key) const
{
	CLongoptionMap::const_iterator i = long_map.find(key);
	return (i != long_map.end()) ? i->second : 0;
}

CCmdOption *CCmdOptionListData::find_option(char key) const
{
	CShortoptionMap::const_iterator i = short_map.find(key);
	return (i != short_map.end()) ? i->second : 0;
}

void CCmdOptionListData::set_current_group(const string& name)
{
	current_group = options.find(name);
	if (current_group == options.end()) {
		options[name] = vector<PCmdOption>();
		current_group = options.find(name);
	}
}

void CCmdOptionListData::add(const string& group, PCmdOption opt)
{
	if (options.find(group) == options.end()) 
		options[group] = vector<PCmdOption>();
	options[group].push_back(opt); 
}

CHistoryRecord CCmdOptionListData::get_values() const
{
	CHistoryRecord result;
	for(auto o_i = options.begin(); o_i != options.end(); ++o_i)
		for(auto g_i = o_i->second.begin(); g_i != o_i->second.end(); ++g_i) 
			result[(*g_i)->get_long_option()] = (*g_i)->get_value_as_string();
	return result;
}


vector<const char *> CCmdOptionListData::has_unset_required_options() const  
{
	vector<const char *> result; 
	for(auto o_i = options.begin(); o_i != options.end(); ++o_i)
		for(auto g_i = o_i->second.begin(); g_i != o_i->second.end(); ++g_i)
			if ((*g_i)->is_required())
				result.push_back((*g_i)->get_long_option()); 
	return result; 
}


#ifdef HAVE_LIBXMLPP
using xmlpp::Element; 
void CCmdOptionListData::print_help_xml(const char *name_help, bool has_additional) const
{
	unique_ptr<xmlpp::Document> doc(new xmlpp::Document);
	
	Element* nodeRoot = doc->create_root_node("program");
	Element* program_name = nodeRoot->add_child("name"); 
	program_name->set_child_text(name_help); 

	Element* program_group = nodeRoot->add_child("section"); 
	program_group->set_child_text(m_program_group); 
	Element* description = nodeRoot->add_child("description"); 
	description->set_child_text(m_general_help); 
	Element* basic_usage = nodeRoot->add_child("basic_usage"); 
	stringstream usage_text; 
	usage_text << " " << name_help << " "; 

	for (auto g = options.begin(); g != options.end(); ++g) {
		Element* group = nodeRoot->add_child("group"); 
		group->set_attribute("name", g->first); 
		
		for (auto iopt= g->second.begin(); iopt != g->second.end(); ++iopt) {
			const CCmdOption& opt = **iopt; 
			if (opt.get_long_option() == string("help-xml")) 
				continue; 
			
			Element* option = group->add_child("option"); 
			option->set_attribute("short", to_string<char>(opt.get_short_option()));
			option->set_attribute("long", opt.get_long_option());
			option->set_attribute("required", to_string<bool>(opt.is_required())); 
			option->set_attribute("default", opt.get_value_as_string()); 
			option->set_child_text(opt.long_help()); 

			
			if (opt.is_required()) {
				if (opt.get_short_option())
					usage_text << "-" << opt.get_short_option() << " &lt;" 
						   << opt.get_long_option() << "&gt; "; 
				else
					usage_text << "--" << opt.get_long_option() 
						   << " &lt;value&gt; ";
			}
		}
	}
	usage_text << "[options]"; 
	if (has_additional) 
		usage_text << " &lt;Additional parameters&gt;"; 
	basic_usage->set_child_text(usage_text.str()); 

	Element* example = nodeRoot->add_child("Example");
	example->set_child_text(m_program_example_descr); 
	Element* example_code = example->add_child("Code"); 
	example_code->set_child_text(m_program_example_code); 
	

	cout << doc->write_to_string_formatted();
	cout << "\n"; 
}
#endif 

/**
   This help printing is a mess ...
 */
void CCmdOptionListData::print_help(const char *name_help, bool has_additional) const
{
	const size_t max_opt_width = 30;
	
	size_t max_width = 70;
#ifdef HAVE_SYS_IOCTL_H
	struct winsize ws; 
	if (ioctl(0,TIOCGWINSZ,&ws)==0) {
		max_width = ws.ws_col;
		if (max_width < max_opt_width + 20) 
			max_width = max_opt_width + 20; 
		
	} 
	if (max_width > 100) 
		max_width = 100; 
#endif
	CFixedWidthOutput console(clog, max_width); 

	vector<string> opt_table;
	vector<string> help_table;
	
	console.write("\nProgram group:  "); 
	console.write(m_program_group); 

	console.push_offset(4); 
	console.write("\n\n"); 
	console.write(m_general_help);
	console.pop_offset();
	console.newline(); 
	
	stringstream usage_text; 
	usage_text <<name_help << " "; 
	
	
	size_t opt_size = 0;
	clog << setiosflags(ios_base::left);
	for (auto i = options.begin(); i != options.end(); ++i) {

		if (i->second.empty()) 
			continue;
		stringstream group;
		group << "\n" << i->first; 
		opt_table.push_back(group.str());
		help_table.push_back("  ");
		
		for (auto g_i = i->second.begin(); g_i != i->second.end(); ++g_i) {
			stringstream opt;
			stringstream shelp;

			const PCmdOption& k = *g_i;
			k->get_opt_help(opt);

			opt_table.push_back(opt.str());
			size_t size = opt.str().size();
			if (opt_size < size)
				opt_size = size;

			k->get_long_help(shelp);
			help_table.push_back(shelp.str());

			if (k->is_required()) {
				if (k->get_short_option())
					usage_text << "-" << k->get_short_option() << " <" << k->get_long_option() << "> "; 
				else
					usage_text << "--" << k->get_long_option() << " <value> ";
			}


		}
	}
	if (opt_size > max_opt_width)
		opt_size = max_opt_width;

	usage_text << "[options]"; 
	if (has_additional) {
		usage_text << " [<" << m_free_parametertype <<">]"
			   << " [<" << m_free_parametertype <<">]"
			   << " ..."; 
	}

	console.write("\nBasic usage:"); 
	console.push_offset(4); 
	console.newline(); 
	console.push_offset(4); 
	console.set_linecontinue(true); 
	console.write(usage_text.str()); 
	console.set_linecontinue(false); 
	console.pop_offset(); 
	console.pop_offset(); 
	console.newline(); 

	console.write("\nThe program supports the following command line options:\n");

		
	auto t  = opt_table.begin();
	for (auto i = help_table.begin(); i != help_table.end(); ++i, ++t) {
		console.write(*t); 
		console.push_offset(opt_size+1); 
		if (t->length() > opt_size) 
			console.newline(); 
		else 
			for (size_t i = t->length(); i <= opt_size; ++i) 
				console.write(" ");
		console.write(*i);
		console.pop_offset(); 
		console.newline(); 
	}

	if (!m_program_example_descr.empty() && !m_program_example_code.empty()) { 
		console.newline();
		console.push_offset(2);
		console.write("Example usage:\n"); 
		console.write(m_program_example_descr); 
		console.push_offset(2);
		console.newline(); 
		console.newline(); 
		console.write(name_help); 
		console.write(" "); 
		console.set_linecontinue(true); 
		console.push_offset(4);
		console.write(m_program_example_code);
		console.set_linecontinue(false); 
		console.reset_offset(); 
		console.newline();
	}
	
	console.write("\n"); 
	console.push_offset(2);
	console.write("Copyright:\n");
	console.write(g_basic_copyright);
	console.pop_offset(); 
	console.write("\n");
	clog << setiosflags(ios_base::right);

}

void CCmdOptionListData::print_usage(const char *name) const
{
	clog << "Usage:\n";
	clog << "  " << name << " ";
	for (COptionsMap::const_iterator i = options.begin();
	     i != options.end(); ++i) {
		COptionsGroup::const_iterator g_i = i->second.begin();
		COptionsGroup::const_iterator g_e = i->second.end();

		while (g_i != g_e) {

			const PCmdOption& k = *g_i;
			k->print_short_help(clog);
			++g_i;
		}
	}
	clog << '\n';
}

CCmdOptionList::CCmdOptionList(const SProgramDescrption& description):
	m_impl(new CCmdOptionListData(description))
{
}	

CCmdOptionList::CCmdOptionList(const string& general_help):
	m_impl(new CCmdOptionListData(general_help))
{
}

CCmdOptionList::CCmdOptionList():
	m_impl(new CCmdOptionListData("This is a MIA toolchain program."))
{
}

void CCmdOptionList::push_back(PCmdOption opt)
{
	m_impl->add(opt);
}

void CCmdOptionList::add(PCmdOption opt)
{
	m_impl->add(opt);
}

void CCmdOptionList::add(const std::string& table, PCmdOption opt)
{
	m_impl->add(table, opt);
}

void CCmdOptionList::set_group(const std::string& group)
{
	m_impl->set_current_group(group); 
}

int CCmdOptionList::handle_shortargs(const char *arg, size_t /*argc*/, const char *args[])
{
	bool bool_options_only = false;
	do {
		CCmdOption *opt = m_impl->find_option(*arg);
		if (!opt ) {
			if ( bool_options_only ) {
				throw invalid_argument(string("bad flag combination:'-") + 
						       string(arg) + string("'"));
			}
			return -1;
		}

		++arg;
		size_t nargs = opt->get_needed_args();
		if (*arg) {
			switch (nargs) {
			case 0:
				opt->set_value(NULL);
				bool_options_only = true;
				break;
			case 1:
				opt->set_value(arg);
				bool_options_only = false;
				break;
			default:
				throw invalid_argument("need a space between option and "
						       "parameter for multiple parameters");
			}
		}else {
			opt->set_value(args[0]);
			return nargs;
		}
	} while (*arg && bool_options_only);

	return 0;
}

CCmdOptionList::EHelpRequested
CCmdOptionList::parse(size_t argc, const char *args[])
{
	return do_parse(argc, args, false);
}


CCmdOptionList::EHelpRequested
CCmdOptionList::parse(size_t argc, char *args[])
{
	return do_parse(argc, (const char **)args, false);
}


CCmdOptionList::EHelpRequested
CCmdOptionList::parse(size_t argc, const char *args[], const string& additional_type)
{
	m_impl->m_free_parametertype = additional_type; 
	return do_parse(argc, args, true);
}

CCmdOptionList::EHelpRequested
CCmdOptionList::parse(size_t argc, char *args[], const string& additional_type)
{
	m_impl->m_free_parametertype = additional_type; 
	return do_parse(argc, (const char **)args, true);
}

CCmdOptionList::EHelpRequested
CCmdOptionList::do_parse(size_t argc, const char *args[], bool has_additional)
{

	size_t idx = 1;
	while (idx < argc ) {
		const char *cur_arg = args[idx++];
		const char *ccarg = cur_arg;
		size_t remaining_args = argc - idx;

		// if we don't scan an option, we deal with a left over argument
		if (*ccarg != '-') {
			m_impl->remaining.push_back(cur_arg);
			continue;
		}

		// we found a singular dash, this is also stored as a left over argument
		++ccarg;
		if (!*ccarg) {
			m_impl->remaining.push_back(cur_arg);
			continue;
		}

		// now check if this is a long option 
		if( *ccarg == '-') { 
			++ccarg;
			CCmdOption *opt = m_impl->find_option(ccarg);
			if (opt) {
				size_t nargs = opt->get_needed_args();
				// currently onyl one argument value is supported
				assert(nargs <= 1); 
				if (remaining_args < nargs ) {
					THROW(invalid_argument, opt->get_long_option() 
					      << ": requires " 
					      << nargs << " arguments, but only " << nargs 
					      << " remaining.");
				}
				
				opt->set_value(args[idx]);
				idx += nargs;
				continue;
			}
		} else {
			int r = handle_shortargs(ccarg, argc - idx, &args[idx]);
			if (r >= 0) {
				idx +=  r;
				continue;
			}
		}
		// current option was not found
		m_impl->remaining.push_back(cur_arg);

	}

	const char *name_help = strrchr(args[0], '/'); 
	name_help  = name_help ? name_help + 1 : args[0]; 

	if (m_impl->help) {
		m_impl->print_help(name_help, has_additional);
		return hr_help;
#ifdef HAVE_LIBXMLPP
	}else if (m_impl->help_xml) {
		m_impl->print_help_xml(name_help, has_additional);
		return hr_help_xml;
#endif 
	} else if (m_impl->usage) {
		m_impl->print_usage(name_help);
		return hr_usage;
	} else if (m_impl->copyright) {
		::print_full_copyright(name_help);
		return hr_copyright;
	}

	cverb.set_verbosity(m_impl->verbose);
	if (!has_additional && !m_impl->remaining.empty()) {
		stringstream msg; 
		msg << "Unknown options given: "; 
		for (auto i = m_impl->remaining.begin(); m_impl->remaining.end() != i; ++i)
			msg << " '" << *i << "' "; 
		throw invalid_argument(msg.str());
	}
	
	auto unset_but_required = m_impl->has_unset_required_options(); 
	if (!unset_but_required.empty()) {
		stringstream msg; 
		if (unset_but_required.size() > 1) 
			msg << "Some required options were not set:"; 
		else
			msg << "A required options was not set:"; 
		for (auto i = unset_but_required.begin(); unset_but_required.end() != i; ++i)
			msg << " '--" << *i << "' "; 
		msg << "\n"; 
		msg << "run '" << args[0] << " --help' for more information\n"; 
		msg << g_basic_copyright; 
		throw invalid_argument(msg.str());
	}
	return hr_no; 
}

const vector<const char *>& CCmdOptionList::get_remaining() const
{
	return m_impl->remaining;
}

CCmdOptionList::~CCmdOptionList()
{
	delete m_impl;
}

PCmdOption EXPORT_CORE make_opt(std::string& value, const std::set<std::string>& set,
				const char *long_opt, char short_opt,
				const char *long_help, const char *short_help, 
				CCmdOption::Flags flags)
{
	return PCmdOption(new CCmdSetOption(value, set, short_opt, long_opt, 
					    long_help, short_help, flags ));
}

PCmdOption EXPORT_CORE make_opt(std::string& value, const std::set<std::string>& set,
                                const char *long_opt, char short_opt, 
				const char *long_help, 
				CCmdOption::Flags flags)
{
	return PCmdOption(new CCmdSetOption(value, set, short_opt, long_opt, 
					    long_help, long_opt, flags ));
}


CHistoryRecord CCmdOptionList::get_values() const
{
	return m_impl->get_values();
}

CHelpOption::CHelpOption(Callback *cb, char short_opt, const char *long_opt, const char *long_help):
	CCmdOption(short_opt, long_opt, long_help, NULL, not_required), 
	m_callback(cb)
{
}

void CHelpOption::print(std::ostream& os) const
{
	m_callback->print(os);
}

bool CHelpOption::do_set_value(const char */*str_value*/)
{
	print(clog); 
	exit(0); 
}
size_t CHelpOption::do_get_needed_args() const
{
	return 0; 
}

void CHelpOption::do_get_long_help(std::ostream& os) const
{
}

void CHelpOption::do_write_value(std::ostream& /*os*/) const
{
}

CCmdFlagOption::CCmdFlagOption(int& val, const CFlagString& map, char short_opt, 
			       const char *long_opt, const char *long_help, 
			       const char *short_help, 
			       CCmdOption::Flags flags):
	CCmdOption(short_opt, long_opt, long_help,short_help, flags),
	m_value(val),
	m_map(map)
{
}

void CCmdFlagOption::do_write_value(std::ostream& os) const
{
	os << m_map.get(m_value);
}

void CCmdFlagOption::do_get_long_help(std::ostream& os) const
{
	os << " supported flags:(" <<m_map.get_flagnames() << os << ")";
}

const std::string CCmdFlagOption::do_get_value_as_string() const
{
	return m_map.get(m_value);
}


bool CCmdFlagOption::do_set_value(const char *str_value)
{
	m_value = m_map.get(str_value);
	return true;
}

size_t CCmdFlagOption::do_get_needed_args() const
{
	return 1;
}

PCmdOption EXPORT_CORE make_opt(int& value, const CFlagString& map, const char *long_opt, 
				char short_opt,const char *long_help, 
				const char *short_help, CCmdOption::Flags flags)
{
	return PCmdOption(new CCmdFlagOption(value, map, short_opt, long_opt,
                          long_help, short_help, flags ));
}


PCmdOption EXPORT_CORE make_help_opt(const char *long_opt, char short_opt, 
				     const char *long_help, CHelpOption::Callback *cb)
{
	return PCmdOption(new CHelpOption(cb, short_opt, long_opt, long_help));
}

NS_MIA_END
