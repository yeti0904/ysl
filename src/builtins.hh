#ifndef YSL_BUILTINS_HH
#define YSL_BUILTINS_HH

#include "_components.hh"
class Environment;

#define YSL_FUNCTION(F) \
	std::vector <int> F(const std::vector <std::string>&, Environment&)

namespace STD {
	YSL_FUNCTION(Print);
	YSL_FUNCTION(PrintLn);
	YSL_FUNCTION(Exit);
	YSL_FUNCTION(Run);
	YSL_FUNCTION(Goto);
	YSL_FUNCTION(GotoIf);
	YSL_FUNCTION(Wait);
	YSL_FUNCTION(Cmp);
	YSL_FUNCTION(Var);
	YSL_FUNCTION(Load);
	YSL_FUNCTION(Size);
	YSL_FUNCTION(Getch);
	YSL_FUNCTION(Input);
	YSL_FUNCTION(Putch);
	YSL_FUNCTION(SetSize);
	YSL_FUNCTION(GoSub);
	YSL_FUNCTION(GoSubIf);
	YSL_FUNCTION(Return);
	YSL_FUNCTION(Debug);
	YSL_FUNCTION(Swap);
	YSL_FUNCTION(Gt);
	YSL_FUNCTION(Lt);
	YSL_FUNCTION(Pow);
	YSL_FUNCTION(StringArray);
	YSL_FUNCTION(Not);
	YSL_FUNCTION(And);
	YSL_FUNCTION(Split);
	YSL_FUNCTION(Or);
	YSL_FUNCTION(IsNum);
	YSL_FUNCTION(Atoi);
	YSL_FUNCTION(Itoa);
	YSL_FUNCTION(LoadEnd);
	YSL_FUNCTION(Error);
	YSL_FUNCTION(Sqrt);
	YSL_FUNCTION(Local);
}

#endif
