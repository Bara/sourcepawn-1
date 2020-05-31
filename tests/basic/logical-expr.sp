/* 
   These were mostly generated from:

   <start> ::= "printnum(" <expr> ");";
   <value> ::= "a" | "b";
   <op> ::= " && " | " || ";
   <expr> ::= "(" <noparenexpr> ")" | <expr> <op> <expr> | <value>;
   <noparenexpr> ::= <expr> <op> <expr> | <value>;
*/

#include <shell>

public void main() {
	bool a = true;
	bool b = false;

	printnum(b && a || a && a);
	printnum((b && a || b && b) && (b && b || b && b));
	printnum(((a || b) && (b || b) || (b || b) && (b || b)) && ((b || b) && (b || b) || (b || b) && (b || b)));

	printnum(((b) && (a && a) && b));
	printnum((a || a && a && (a) && b && ((b) || b && b)));
	printnum(((a) || a || b && a));
	printnum((b || (a) && (a || a) || (a) || (b) || (b && (a) && (a || (b) && (b) || b) || b) || a));
	printnum((a) || b || a || (b || a));
	printnum((((a && (b)) && ((b) || a && (b || ((a) || (b || (a && a)) && ((((a) && (b)) || a || ((a) || (b) || a && a && (b) || a && b) && (a) && a && a && (b) || (a)) || (b) || b && (b || a))) || ((a && (a) || (a) || b) || b && (((a) || (b) || a) && (a && a || (b) && ((((a || (a) || (b) || a || a) && a || (b) || (a) && a || a && (a) && ((b && a) || (b)) || (a)) || a) || ((a) || a) || b || (a && a) || a || b && a || a) && a || a || a || (b)))) || (a) || a)) && (a) || b && (a || b) || a) && a));
	printnum((a) && b && a || b && a);
	printnum((a) && ((b) && (b) && b && b || a && (a) || b) || b && b);
	printnum((((b) || a) && ((a) && (b) && ((b) || a && b && a || (a) && (a || (a) || b || b || a || (b || b || (b))) && (a) || a || ((((b && ((((b && a || (a)) && b) && b || (a && a || (b) || a && (a) && (b && b && b) && b)) && b) && ((b || ((b) || b && (b)) && b && b) && b)) || a && a) || (b) || b) || a && (a) && b) || a || a || b)) && ((b) && b && ((a || (a) && (a) || b && ((b || (a)) && b && b) || a || b || (a && b) || (a) && a && b) && (b || (b) || b && ((((b) || b && a) || (b)) && b) && (a) && a || (a || b && (a || b || a) && ((a) && a) || b || (b && (a || b && (a) || a || a && b || (b) && a || b || (b) && ((b) && b || b && ((a) && a && a || b && (a) || (a || (b) && a && b || ((b) || (a && (b)) && (b)) && a || (b)) || (a || ((b) || a) || b && (b)) || a && (a && (((a) || a) || b) || (b) && b || ((b) || (a || b) || b) || b || b && b && (a) || b) && b && (a && (b && ((a) || a || a)))))) || a && ((a) || b || b || b) && (a) && (b || (((a) || b && a && (b)) || b && (a && a) || a && (b)) && ((a) || ((a) && (((b || (((b) && b || b && (a) || b || a && a || (a)) || a && b || (a) || (a))) && b) && a && a && ((b) || a) && a || (a) || a || b && ((((a) && b && a) || ((b) && (b || a))) || (((b || ((a) || (a)) || ((b && (a) || (b || (b))) || (a))) || b) || b || a) && b) || (b)))) || a || b) || (b))) && (((a) && (a) || b) && ((a) && ((a) && b || a && (a) || b || (a && a && b || b) || b || (a) || a))) || (a) || b && (b || ((b) || a) && (a) && b) && b)) || a || a && (a) || (b)) && a && (b) || (a)));
	printnum((a && (a) && (b || b || (a) && (a) && (b && (b)) && (a) || b && a || b)));
	printnum((b && (((b && (b && ((a && b) || (b) && (b && b) || a && b) || a && ((a) || a || (b) || b || b && b) && a || (a && (((a) || (a && a || a || a) && a) && a && a) || a && (b))) || b) && a && b && (a) || (a)) || a || (b && a))) || b);
	printnum((b || (a) || a) || (b) && (b) || b && (b) && b && (a && b && ((b) || (b) || a)));
	printnum((a || (a && (b) || b && a)));
	printnum(((b) || (b || ((a || a) || (b && a) || a || b || (a) || b && a) || (b)) && ((a) || a || a) || (a && a) && a) || (a));
	printnum(((b) || ((a) && ((b && (b || (b) || a || (a) && ((a) && b) && (b)) || (a) && (b) || a && (a) && b) || (a) || a) && b && a || b || (b) || b && (((a || b) || b && b || a && b && ((b && (a || (b && ((b && a && b || (((b) && b) || b) || b) && a || b && a && a || b && ((b) && (b)) || (a) && (a) || b || b) && a && a || (a)) || a || (a || b || (b) || b) && (b || b)) || (b)) && a && a) || a || ((b) && (a) || (((a) && ((a) || (a) || b && (a && b))) && a || a && (a) && (a && (b || (a) && b || a && a) || (b && b && ((b) && ((a) && b) || a && b || (b) && (b || ((b) || ((b) && (a) && (a || b) || b && a || (b)) && (b) && a || a && a && b && b) && (a)) || (a))) && a && b) && a || (b && (a)) || (b) && b) || a || (a) || (a)) && a) && (((a) || b && (a || (b))) && b && (((((b || (a)) || b && ((a) || b)) && a || (a || a && b && a && b && (a) || (b) && b && (((b || (a || (a || (a && b || (b || b)) && b) || a)) && (a) || a || a && (((b && b) || b) && b || (b) || (b || (b && b && ((a) || (b) && (b))) || a) && a) || b || a || a) || a || a && (a && (b))))) || ((((b) && (a)) && ((b && a || b) || ((b) && b) || b || a || a)) && b) && (a)) || a) && a) || ((a) && (b && a || ((a || (b) && a) && ((a) || b)))) || b))));
	printnum(((b) || a) && ((b) && a || ((a) && b) && a || a && ((a && (a) && (a || b || (a) && a || (b)) && b || b) && (b)) && ((a) && b)));
	printnum((a) && ((b) || (b)) && b);
	printnum((b || a && b || a || b) && ((a) || (((a) || a || ((a || (b)) && a || ((b) || (b)) || a || (a))) || (a) && ((b) || ((b) || ((b || a && (b) || (b) || (a || (a && ((a) || (b) && (b))) && (a) && (b) || (a)) || a && ((b) || (a && (a) || b)) && (b)) || (b)))) && (b || b) && (b) && a) || b) || a);
	printnum(b && ((a || a) || (((b) && a) || a) || (a && ((a) && (a || b) || (b || (b) || (b) || (b || b || a) && b && b) || b && a || a || a) || (b))));
	printnum(a || a || (b || a || ((a && (b) || (a) && a) || ((b && b && a && a && (b)) && b || (((b) || a || a) && (a)))) || (b) || a && (b)));
}