/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ohm-ast.hpp"

namespace Ohm 
{
	// Data structures for building the Abstract Syntax Tree of an Ohm grammar;
	namespace AST 
	{
		using namespace std;
        
		// make sure that ordering + elements here strictly follows variant-decl:
		const string typenames[] = {
			"name", "terminal", "caseName", "RuleDescr", "SuperGrammar",
			"Base", "Lex", "Pred", "Iter", "Seq", "Params", "Alt", "TopLevelTerm", "RuleBody", "Rule", "Grammar", "Grammars"
		};
		
		string typenameOf(StackItem si)
		{
			return typenames[si.index()];		
		}
	}
};
