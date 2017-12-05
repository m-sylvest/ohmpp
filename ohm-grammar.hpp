#include <pegtl.hpp>

namespace Ohm {

  namespace GRM {

    using namespace tao::TAOCPP_PEGTL_NAMESPACE;

    struct Grammars;
    struct Grammar;
    struct SuperGrammar;
    
		struct Rule;
    struct Rule_Define;
    struct Rule_Override;
    struct Rule_Extend;
		
    struct RuleBody;
		
    struct TopLevelTerm;
		struct TopLevelTerm_CaseName;
		struct TopLevelTerm_Seq;
		
    struct Formals;
    struct Params;
    struct Alt;
    struct Seq;
		
    struct Iter;
    struct Iter_Star;
    struct Iter_Plus;
    struct Iter_Opt;
    struct Iter_Pred;

    struct Pred;
    struct Pred_Not;
    struct Pred_Lookahead;
    struct Pred_Lex;

    struct Lex;
    struct Lex_Lex;
    struct Lex_Base;

    struct Base;
		struct Base_Appl;
		struct Base_Range;
		struct Base_Terminal;
		struct Base_Paren;

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

    // helpers for Ohm - capitalisation / padding;
    template < typename ...T >
    struct STAR : pad< star<T ...>, space_ > {};

    template < typename ... T >
    struct SEQ : pad< seq<T ...>, space_ > {};

    template < typename ... T >
    struct SOR : pad< sor<T ...>, space_ > {};

    template < typename ... T >
    struct OPT : pad< opt<T ...>, space_ > {};

    // NonEmptyListOf:
    template < typename T, typename P >
    struct NonEmptyListOf : pad< list< T, P >, space_ > {};

    // ListOf:
    template < typename T, typename P >
    struct ListOf : opt< NonEmptyListOf<T,P> > {};

    //
    // Here comes the Ohm grammar in PEGTL parlance:
    //

    //  Grammars
    //    = Grammar*
    struct Grammars : pad< until<eof,Grammar>, blank> {};

    //  Grammar
    //    = ident SuperGrammar? "{" Rule* "}"
    struct Grammar  : SEQ< ident, opt<SuperGrammar>, one<'{'>, star<Rule>, one<'}'> > {};

    //  SuperGrammar
    //    = "<:" ident
    struct SuperGrammar : SEQ< string<'<',':'>, ident > {};

    //  Rule
    //    = ident Formals? ruleDescr? "="  RuleBody  -- define
    //    | ident Formals?            ":=" RuleBody  -- override
    //    | ident Formals?            "+=" RuleBody  -- extend
		//
		// rewritten to Rule = ident Formals? ( Rule_Define | Rule_Override | Rule_Extend )
		//
		struct Rule : SEQ< ident, opt<Formals>, sor< Rule_Define, Rule_Override, Rule_Extend > > {}; 
		
		struct Rule_Define	: SEQ<OPT<ruleDescr>, one<'='>, RuleBody > {};
		struct Rule_Override: SEQ<			string<':','='>,		RuleBody > {};
		struct Rule_Extend	: SEQ<			string<'+','='>,		RuleBody > {};

    //  RuleBody
    //    = "|"? NonEmptyListOf<TopLevelTerm, "|">
    struct RuleBody : SEQ< opt< one<'|'> >, NonEmptyListOf< TopLevelTerm, one<'|'> > > {};

    //  TopLevelTerm
    //    = Seq caseName  -- inline
    //    | Seq
		//
		// rewritten to 
		//   TopLevelTerm = Seq caseName?  
		//
		struct TopLevelTerm : SEQ< Seq, opt<caseName> > {}; 

    //  Formals
    //    = "<" ListOf<ident, ","> ">"
    struct Formals : SEQ< 
			one<'<'>, 
			ListOf< ident, one<','> >,
			one<'>'>
    > {};

    //  Params
    //    = "<" ListOf<Seq, ","> ">"
    struct Params : SEQ<
			one<'<'>,
			ListOf< Seq, one<','> >,
			one<'>'>
    > {};

    //  Alt
    //    = NonEmptyListOf<Seq, "|">
    struct Alt : NonEmptyListOf< Seq, one<'|'> > {};

    //  Seq
    //    = Iter*
    struct Seq : STAR<Iter> {};
		
    //  Iter
    //    = Pred "*"  -- star
    //    | Pred "+"  -- plus
    //    | Pred "?"  -- opt
    //    | Pred
		//
		// -- for the sake of stack-based Pred's rewritten to
		//
		//  Iter = Pred ( "*" | "+" | "?" )?
		//
    struct Iter : 
			SEQ	<	Pred, 
						opt<
							sor<
								one<'*'>,one<'+'>,one<'?'> 
							>
						>
					> {};
#if 0
		struct Iter_Star : seq< Pred, one<'*'> > {};
		struct Iter_Plus : seq< Pred, one<'+'> > {};
		struct Iter_Opt  : seq< Pred, one<'?'> > {};
#endif    

    //  Pred
    //    = "~" Lex  -- not
    //    | "&" Lex  -- lookahead
    //    | Lex
    struct Pred : SOR< Pred_Not, Pred_Lookahead, Pred_Lex > {};
		
		struct Pred_Not				: seq< one<'~'>, Lex > {};
		struct Pred_Lookahead	: seq< one<'&'>, Lex > {};
		
		struct Iter_Pred : seq<Pred> {};

    //  Lex
    //    = "#" Base  -- lex
    //    | Base
    struct Lex : sor< Lex_Lex, Lex_Base > {};
		
		struct Lex_Lex : seq< one<'#'>, Base > {};
	
		struct Pred_Lex       : seq<Lex> {};

    //  Base
    //    = ident Params? ~(ruleDescr? "=" | ":=" | "+=")  -- application
    //    | oneCharTerminal ".." oneCharTerminal           -- range
    //    | terminal                                       -- terminal
    //    | "(" Alt ")"                                    -- paren
    struct Base : SOR< Base_Appl, Base_Range, Base_Terminal, Base_Paren > {};

		// Base_terminal has been moved below 'struct terminal'
		
		struct Base_Paren : SEQ< one<'('>, Alt, one<')'> > {};

		struct Base_Range: SEQ< oneCharTerminal, string<'.','.'>, oneCharTerminal > {};
		
		struct Base_Appl : 
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
			> {};
			
		struct Lex_Base: seq<Base> {};

    //  ruleDescr  (a rule description)
    //    = "(" ruleDescrText ")"
    struct ruleDescr : seq< one<'('>, ruleDescrText, one<')'> >{};

    //  ruleDescrText
    //    = (~")" any)*
    struct ruleDescrText : until< one<')'>, any > {};

    //  caseName
    //    = "--" (~"\n" space)* name (~"\n" space)* ("\n" | &"}")
    struct caseName : seq<
			string<'-','-'>,
			star< 
				seq< 
					not_at< one<'\n'> >, 
					space_
				> 
			>,
//			star< 
//				seq< alpha > 
//			>,
			name,
			star< 
				seq< 
					not_at< one<'\n'> >, 
					space_
				> 
			>,
			sor<
				one<'\n'>,
				at< one<'}'> >
			>
    > {};

    //  name  (a name)
    //    = nameFirst nameRest*
    struct name : seq< nameFirst, star<nameRest> > {};

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
    struct ident : seq<name> {};

    //  terminal
    //    = "\"" terminalChar* "\""
    struct terminal : seq< one<'"'>, star<terminalChar>, one<'"'> > {};

		struct Base_Terminal: seq<terminal> {};

    //  oneCharTerminal
    //    = "\"" terminalChar "\""
    struct oneCharTerminal : seq< one<'"'>, terminalChar, one<'"'> > {};

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
        string<'\\', '\\'>,
        string<'\\', '"' >,
        string<'\\', '\''>,
        string<'\\', 'b' >,
        string<'\\', 'n' >,
        string<'\\', 'r' >,
        string<'\\', 't' >,
//      seq< one<'\\'>, one<'u'>, xdigit, xdigit, xdigit, xdigit >,
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

  };

};
