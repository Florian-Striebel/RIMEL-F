






























namespace std
{

class exception
{
public:
exception() throw();
exception(const exception&) throw();
exception& operator=(const exception&) throw();
virtual ~exception();
virtual const char* what() const throw();
};





class bad_alloc: public exception
{
public:
bad_alloc() throw();
bad_alloc(const bad_alloc&) throw();
bad_alloc& operator=(const bad_alloc&) throw();
~bad_alloc();
virtual const char* what() const throw();
};




class bad_cast: public exception {
public:
bad_cast() throw();
bad_cast(const bad_cast&) throw();
bad_cast& operator=(const bad_cast&) throw();
virtual ~bad_cast();
virtual const char* what() const throw();
};




class bad_typeid: public exception
{
public:
bad_typeid() throw();
bad_typeid(const bad_typeid &__rhs) throw();
virtual ~bad_typeid();
bad_typeid& operator=(const bad_typeid &__rhs) throw();
virtual const char* what() const throw();
};

class bad_array_new_length: public bad_alloc
{
public:
bad_array_new_length() throw();
bad_array_new_length(const bad_array_new_length&) throw();
bad_array_new_length& operator=(const bad_array_new_length&) throw();
virtual ~bad_array_new_length();
virtual const char *what() const throw();
};


}

