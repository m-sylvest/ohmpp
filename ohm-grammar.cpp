
#include <pegtl.hpp>

using namespace tao::TAOCPP_PEGTL_NAMESPACE;

struct Grammars;
struct Grammar;
struct SuperGrammar;
struct Rule;
struct RuleBody;
struct TopLevelTerm;
struct Formals;
struct Params;
struct Alt;
struct Seq;
struct Iter;
struct Pred;
struct Lex;
struct Base;
struct ruleDescr;
struct ruleDescrText;
struct caseName;
struct name;
struct nameFirst;
struct nameRest;
struct ident;
struct terminal;
struct oneCharTerminal;
struct terminalChar;
struct escapeChar;
struct space_;
struct comment;
struct tokens;
struct token;
struct operator_;
struct punctuation;

//  Grammars
//    = Grammar*
struct Grammars : star<Grammar> {};

//  Grammar
//    = ident SuperGrammar? "{" Rule* "}"
struct Grammar  : seq< ident, opt<SuperGrammar>, one<'{'>, star<Rule>, one<'}'> > {};

//  SuperGrammar
//    = "<:" ident
struct SuperGrammar : seq< one<'<'>, one<':'>, ident > {};

//  Rule
//    = ident Formals? ruleDescr? "="  RuleBody  -- define
//    | ident Formals?            ":=" RuleBody  -- override
//    | ident Formals?            "+=" RuleBody  -- extend
struct Rule : sor<
	seq< ident, opt<Formals>, opt<ruleDescr>,           one<'='>, RuleBody >,
	seq< ident, opt<Formals>,                 one<':'>, one<'='>, RuleBody >,
	seq< ident, opt<Formals>,                 one<'+'>, one<'='>, RuleBody >
> {};

//  RuleBody
//    = "|"? NonemptyListOf<TopLevelTerm, "|">
struct RuleBody : seq< 
  opt< one<'|'> >, 
  plus< list< TopLevelTerm, one<'|'> > >
> {};

//  TopLevelTerm
//    = Seq caseName  -- inline
//    | Seq
struct TopLevelTerm : sor< seq< Seq, caseName >, Seq > {}; 

//  Formals
//    = "<" ListOf<ident, ","> ">"
struct Formals : seq< 
  one<'<'>, 
  list< ident, one<','> >,
  one<'>'>
> {};

//  Params
//    = "<" ListOf<Seq, ","> ">"
struct Params : seq<
	one<'<'>,
	list< Seq, one<','> >,
	one<'>'>
> {};

//  Alt
//    = NonemptyListOf<Seq, "|">
struct Alt : list< Seq, one<'|'> > {};

//  Seq
//    = Iter*
struct Seq : star<Iter> {};

//  Iter
//    = Pred "*"  -- star
//    | Pred "+"  -- plus
//    | Pred "?"  -- opt
//    | Pred
struct Iter : sor<
	seq< Pred, one<'*'> >,
	seq< Pred, one<'+'> >,
	seq< Pred, one<'?'> >,
	Pred
> {};

//  Pred
//    = "~" Lex  -- not
//    | "&" Lex  -- lookahead
//    | Lex
struct Pred : sor<
	seq< one<'~'>, Lex >,
	seq< one<'&'>, Lex >,
  Lex
> {};

//  Lex
//    = "#" Base  -- lex
//    | Base
struct Lex : sor<
	seq< one<'#'>, Base >,
  Base
> {};

//  Base
//    = ident Params? ~(ruleDescr? "=" | ":=" | "+=")  -- application
//    | oneCharTerminal ".." oneCharTerminal           -- range
//    | terminal                                       -- terminal
//    | "(" Alt ")"                                    -- paren
struct Base : sor<
	seq< 
		ident, 
		opt<Params>, 
		not_at< 
			sor< 
				seq< 
					opt<ruleDescr>, 
					one<'='> 
				>, 
				string<':', '=' >, 
				string<'+', '='> 
			> 
		>
	>,
  seq< oneCharTerminal, string<'.','.'>, oneCharTerminal >,
	terminal,
	seq< one<'('>, Alt, one<')'> >
> {};

//  ruleDescr  (a rule description)
//    = "(" ruleDescrText ")"
struct ruleDescr : seq< one<'('>, ruleDescrText, one<')'> >{};

//  ruleDescrText
//    = (~")" any)*
struct ruleDescrText : star< seq< 
	not_at< one<')'> >,
	any
> >{};

//  caseName
//    = "--" (~"\n" space)* name (~"\n" space)* ("\n" | &"}")
struct caseName : seq<
	string<'-','-'>,
  star< seq< 
			not_at< one<'\n'> >, 
			space_
	> >,
	name,
  star< seq< 
			not_at< one<'\n'> >, 
			space_
	> >,
	sor<
		one<'\n'>,
		at< one<'}'> >
  >
> {};

//  name  (a name)
//    = nameFirst nameRest*
struct name : seq< nameFirst, nameRest > {};

//  nameFirst
//    = "_"
//    | letter
struct nameFirst : sor< one<'_'>, alpha > {};

//  nameRest
//    = "_"
//    | alnum
struct nameRest : sor< one<'_'>, alnum > {};

//  ident  (an identifier)
//    = name
struct ident : name {};

//  terminal
//    = "\"" terminalChar* "\""
struct terminal : seq< one<'\\'>, star<terminalChar>, one<'\\'> > {};

//  oneCharTerminal
//    = "\"" terminalChar "\""
struct oneCharTerminal : seq< one<'\\'>, terminalChar, one<'\\'> > {};

//  terminalChar
//    = escapeChar
//    | ~"\\" ~"\"" ~"\n" any
struct terminalChar : sor<
	escapeChar,
	seq< not_at<one<'\\'>>, not_at<one<'"'>>, not_at<one<'\n'>>, any >
> {};

//  escapeChar  (an escape sequence)
//    = "\\\\"                                     -- backslash
//    | "\\\""                                     -- doubleQuote
//    | "\\\'"                                     -- singleQuote
//    | "\\b"                                      -- backspace
//    | "\\n"                                      -- lineFeed
//    | "\\r"                                      -- carriageReturn
//    | "\\t"                                      -- tab
//    | "\\u" hexDigit hexDigit hexDigit hexDigit  -- unicodeEscape
//    | "\\x" hexDigit hexDigit                    -- hexEscape
struct escapeChar : sor<
	one<'\\'>,
	one<'"'>,
	one<'\''>,
	one<'\b'>,
	one<'\n'>,
	one<'\r'>,
	one<'\t'>,
	seq< one<'\\'>, one<'u'>, xdigit, xdigit, xdigit, xdigit >,
	seq< one<'\\'>, one<'x'>, xdigit, xdigit >
> {};

//  space
//   += comment
struct space_ : sor< comment, space > {};

//  comment
//    = "//" (~"\n" any)* "\n"  -- singleLine
//    | "/*" (~"*/" any)* "*/"  -- multiLine
struct comment : sor<
	seq< 
		string< '/', '/' >, 
		star< 
			seq< 
				not_at< one<'\n'> >, 
				any
			>
		>,
		one<'\n'> 
  >,
	seq< 
		string< '/', '*' >, 
		star< 
			seq< 
				not_at< string< '*', '/' > >, 
				any 
			> 
		>,
		string< '*', '/' >
  >
> {};

//  tokens = token*
struct tokens : star< token > {};

//  token = caseName | comment | ident | operator | punctuation | terminal | any
struct token : sor< caseName, comment, ident, operator_, punctuation, terminal, any > {};
	
//  operator = "<:" | "=" | ":=" | "+=" | "*" | "+" | "?" | "~" | "&"
struct operator_ : sor<
	string< '<', ':' >, 
	one< '=' >,
	string< ':', '=' >, 
	string< '+', '=' >, 
	one< '*' >,
	one< '+' >,
	one< '?' >,
	one< '~' >,
	one< '&' >
> {};

//  punctuation = "<" | ">" | "," | "--"
struct punctuation : sor<
	one< '<' >,
	one< '>' >,
	one< ',' >,
	string< '-', '-' >
> {};
