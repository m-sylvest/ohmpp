/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ohm-ast.hpp
 * Author: mikael
 *
 * Created on December 11, 2017, 4:48 AM
 */

#ifndef OHM_AST_HPP
#define OHM_AST_HPP

#include <string>
#include <list>
#include <variant>

namespace Ohm 
{
	// Data structures for building the Abstract Syntax Tree of an Ohm grammar;
	namespace AST 
	{
		using namespace std;
        
		typedef struct { string s; } name;
		typedef struct { string s; } terminal;
		typedef struct { string s; } caseName;
		typedef struct { string s; } SuperGrammar;
		typedef struct { string s; } RuleDescr;

		struct Base;

		typedef struct { bool hashPrefix; Base* base; } Lex;
		typedef struct { string op; Lex* lex; } Pred;
		typedef struct { Pred* pred; string op; } Iter;
		typedef list<Iter *>  Seq;
		 
		typedef struct { Seq *seq; string caseName; } TopLevelTerm;
		typedef list<TopLevelTerm *> RuleBody;
		typedef struct { list<Seq *> l; } Params;
		typedef struct { list<Seq *> l; } Alt;
		typedef struct { 
			enum class Type { Define, Override, Extend  } type;
			string name, rulesDescr;
			RuleBody *ruleBody;
			Params *ruleParms;
		} Rule;

		typedef struct { string name, parent; list<Rule *> *rules; } Grammar;
		typedef list<Grammar *> Grammars;

		typedef struct Base { 
			enum class Type { Appl, Range, Term, Alt } type;
			string name; 
			Params *params; 
			Alt   *alts; 
			string rangeFrom, rangeTo; 
		} Base;

		// The Grand Unified data structure to hold all AST values an types met during compilation:
		typedef variant< 
			name *,
			terminal *,
			caseName *,
			RuleDescr *,
			SuperGrammar *,
			Base *,
			Lex *,
			Pred *,
			Iter *,
			Seq *,
			Params *,
			Alt *,
			TopLevelTerm *,
			RuleBody *,
			Rule *,
			Grammar *,
			Grammars *
		> StackItem;
		
		// make sure that ordering + elements here strictly follows above variant:
		extern const string typenames[];
		
		template< typename T >
		constexpr string typenameOf()
		{
			StackItem si = static_cast<T *>(nullptr);
			return typenames[si.index()];		
		}

		string typenameOf(StackItem si);		
		
	}
};



#endif /* OHM_AST_HPP */

