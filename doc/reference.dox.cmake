# Doxyfile 1.3.4

PROJECT_NAME           = @PACKAGE@
PROJECT_NUMBER         = @LT_RELEASE@
OUTPUT_DIRECTORY       = libdoc
OUTPUT_LANGUAGE        = English
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = NO
STRIP_FROM_PATH        = 
STRIP_FROM_INC_PATH    = @CMAKE_CURRENT_SOURCE_DIR@/..
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
MULTILINE_CPP_IS_BRIEF = NO
INHERIT_DOCS           = YES
DISTRIBUTE_GROUP_DOC   = NO
TAB_SIZE               = 8
ALIASES                = 
OPTIMIZE_OUTPUT_FOR_C  = YES
OPTIMIZE_OUTPUT_JAVA   = NO
SUBGROUPING            = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = YES
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = YES
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
SORT_BRIEF_DOCS        = YES
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       = 
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
QUIET                  = YES
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           = 
INPUT                  =  @CMAKE_CURRENT_SOURCE_DIR@/../doc @CMAKE_CURRENT_SOURCE_DIR@/../mia/core @CMAKE_CURRENT_SOURCE_DIR@/../mia/internal @CMAKE_CURRENT_SOURCE_DIR@/../mia/2d @CMAKE_CURRENT_SOURCE_DIR@/../mia/3d @CMAKE_CURRENT_SOURCE_DIR@/../mia/mesh @CMAKE_CURRENT_SOURCE_DIR@/../mia/template @CMAKE_CURRENT_SOURCE_DIR@/../mia/internal  
FILE_PATTERNS          = *.hh *.doc 
RECURSIVE              = NO
EXCLUDE                = 
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       = 
EXAMPLE_PATH           = 
EXAMPLE_PATTERNS       = 
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             = 
INPUT_FILTER           = 
FILTER_SOURCE_FILES    = NO
SOURCE_BROWSER         = YES
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = YES
REFERENCES_RELATION    = YES
VERBATIM_HEADERS       = YES
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          = 
GENERATE_HTML          = YES
HTML_OUTPUT            = 
HTML_FILE_EXTENSION    = .html 
HTML_TIMESTAMP         = NO
HTML_HEADER            = 
HTML_FOOTER            = 
HTML_STYLESHEET        = 
GENERATE_HTMLHELP      = NO
CHM_FILE               = 
HHC_LOCATION           = 
GENERATE_CHI           = NO
BINARY_TOC             = NO
TOC_EXPAND             = NO
DISABLE_INDEX          = NO
ENUM_VALUES_PER_LINE   = 4
GENERATE_TREEVIEW      = YES
TREEVIEW_WIDTH         = 250
GENERATE_LATEX         = NO
LATEX_OUTPUT           = 
LATEX_CMD_NAME         = latex
MAKEINDEX_CMD_NAME     = makeindex
COMPACT_LATEX          = YES
PAPER_TYPE             = a4wide
EXTRA_PACKAGES         = amsmath \
			 amssymb \
			 amsxtra \
                         amsfonts
LATEX_HEADER           = 
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = YES
LATEX_BATCHMODE        = NO
LATEX_HIDE_INDICES     = NO
GENERATE_RTF           = NO
RTF_OUTPUT             = 
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    = 
RTF_EXTENSIONS_FILE    = 
GENERATE_MAN           = NO 
MAN_OUTPUT             = 
MAN_EXTENSION          = 
MAN_LINKS              = NO
GENERATE_XML           = NO
XML_OUTPUT             = xml
GENERATE_AUTOGEN_DEF   = NO
GENERATE_PERLMOD       = NO
PERLMOD_LATEX          = NO
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX = 
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_ONLY_PREDEF     = YES
SEARCH_INCLUDES        = YES
INCLUDE_PATH           = 
INCLUDE_FILE_PATTERNS  = 
PREDEFINED             = BOOST_CONCEPT_REQUIRES(A,B)=B NS_MIA_BEGIN= NS_MIA_END= __GNUC__=1 MIA_PARAM_UNUSED(X)=X
EXPAND_AS_DEFINED      = 
SKIP_FUNCTION_MACROS   = YES
TAGFILES               = 
GENERATE_TAGFILE       = 
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
PERL_PATH              = 
CLASS_DIAGRAMS         = YES
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = @HAVE_DOT@
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
UML_LOOK               = NO
TEMPLATE_RELATIONS     = YES
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = NO
GRAPHICAL_HIERARCHY    = YES
DOT_IMAGE_FORMAT       = svg
DOT_PATH               = 
DOTFILE_DIRS           = 
MAX_DOT_GRAPH_DEPTH    = 0
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
SEARCHENGINE           = NO
SHOW_NAMESPACES        = NO
DOT_GRAPH_MAX_NODES    = 100 
HTML_TIMESTAMP         = NO


