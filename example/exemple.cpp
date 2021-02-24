#include "convert_map.h"
using namespace std;

struct MyAnnotation
{
    int number;
    MyAnnotation(int n)  { number = n;}

    void printOld(int age){
        if(age >= number)
            cout << " is old";
    }
};

struct print_person
{
    template<class FieldData, class Annotations>
    void operator()(FieldData f, Annotations a, int lenght)
    {
        std::cout << std::endl << f.name() << "=" << f.get() ;
    }
};

class Person
{
public:
REFLECTABLE(
    (std::string) name,
    (int) age,
    (std::string) addres);

    Person(){}
    Person(string name, int age, string addr){
        this->name = name;
        this->age = age;
        this->addres = addr;
    }
};
ANNOTATIONS_ENTITY(Person) = {Entity("person")};
ANNOTATIONS_FIELDS(Person) = {  {"addres", {IgnoreData()}},
                                {"name",   {SpaceReplace("_")}},
                                {"age", {MyAnnotation(60)}} };


int main()
{
    Person p("Tom Petter", 82, "Rua de cima");

    map<string, string> data = to_map(p);
    Person copy = from_map(data);

    Entity* e = Person::annotations::get_entity();
    cout << e->name;

    MyAnnotation* myann = Person::annotations:: get_field("age");
    if(myann)
        myann->printOld(copy.age);

    reflector::visit_each(copy, print_person{});
}
