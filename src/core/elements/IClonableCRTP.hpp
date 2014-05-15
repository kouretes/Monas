#ifndef ICLONABLECRTP_HPP
#define ICLONABLECRTP_HPP

//Curiously Recuring template Pattern Clonable Interface stub
template<class Base,class Derived>
class IClonableCRTP
{
	public:
	Base * Clone() const {return new Derived(dynamic_cast<Derived const&>(*this));};

};

#endif // ICLONABLECRTP_HPP
