// -i using_class.in using_class.hh -- 
class LongName {};
class Name {
public:
    LongName& foo(const LongName *);
    const Name funcName(Name&);
    Name* NameName(Name);
};
