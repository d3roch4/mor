#ifndef MOR_H
#define MOR_H

#include <iostream>
#include <boost/preprocessor.hpp>
#include <utility>
#include <type_traits>
#include <unordered_map>
#include <list>
#include <boost/any.hpp>
#include <chrono>

#define REM(...) __VA_ARGS__
#define EAT(...)

// Retrieve the type
#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
// Strip off the type
#define STRIP(x) EAT x
// Show the type without parenthesis x
#define PAIR(x) REM x

// A helper metafunction for adding const to a type
template<class M, class T>
struct make_const
{
    typedef T type;
};

template<class M, class T>
struct make_const<const M, T>
{
    using type = typename std::add_const<T>::type;
};

struct get_entity_struct {
    std::list<boost::any>& list;
    get_entity_struct(std::list<boost::any>& list) : list(list) {}

    template<class A>
    operator A* (){
        for(boost::any& a: list)
            if(a.type() == typeid(A)){
                A& ann = boost::any_cast<A&>(a);
                return &ann;
            }
        return nullptr;
    }
};


struct get_field_struct {
    const char* name;
    std::unordered_map<std::string, std::list<boost::any>>& fields;

    get_field_struct(const char* name, std::unordered_map<std::string, std::list<boost::any>>& fields) :
        name(name),
        fields(fields){

    }

    template<class A>
    operator A* (){
        auto it = fields.find(name);
        if(it != fields.end())
            for(boost::any& a: it->second)
                if(a.type() == typeid(A))
                    return &boost::any_cast<A&>(a);
        return nullptr;
    }
};


#define REFLECTABLE(...) \
    friend struct reflector; \
    template<int N, class Self> \
    struct field_data {}; \
    BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    public: \
    struct annotations{ \
        static const int __fcount = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
        static std::list<boost::any> _entity; \
        static std::unordered_map<std::string, std::list<boost::any>> _fields; \
        std::list<boost::any>& entity(){ \
            return _entity; \
        } \
        std::unordered_map<std::string, std::list<boost::any>> fields(){ \
            return _fields; \
        } \
        static get_entity_struct get_entity(){ \
            get_entity_struct s(_entity); \
            return s; \
        } \
        template<class E> \
        static void put_entity(const E& e){ \
            boost::any a(e);\
            _entity.push_back(a); \
        } \
        static get_field_struct get_field(const char* name){ \
            get_field_struct s(name, _fields); \
            return s; \
        } \
    }; \

#define REFLECT_EACH(r, data, i, x) \
    PAIR(x){}; \
    template<class Self> \
    struct field_data<i, Self> \
    { \
        typedef  typename make_const<Self, TYPEOF(x)>::type type; \
        Self & self; \
        field_data(Self & self) : self(self) {} \
        \
        typename make_const<Self, TYPEOF(x)>::type & get() \
        { \
            return self.STRIP(x); \
        }\
        std::add_const_t<TYPEOF(x)>& get() const \
        { \
            return self.STRIP(x); \
        }\
        const char* name() const \
        {\
            return BOOST_PP_STRINGIZE(STRIP(x)); \
        } \
        get_field_struct annotation() const \
        { \
            return Self::annotations::get_field(BOOST_PP_STRINGIZE(STRIP(x))); \
        } \
    };

#define ANNOTATIONS_ENTITY(X) std::list<boost::any> \
    X::annotations::_entity

#define ANNOTATIONS_FIELDS(X) std::unordered_map<std::string, std::list<boost::any>> \
    X::annotations::_fields

/**
 * @brief The Entity struct annotation
 * @example ANNOTATIONS_ENTITY(X) = {Entity("name")}
 */
struct Entity
{
    std::unordered_map<std::string, std::string> attributes;
    std::string name;
    Entity(std::string name) : name(name) {}
};

struct Reference
{
    std::string entity;
    std::string field;
    Reference(std::string entity_name, std::string field_name="") :
        entity(entity_name),
        field(field_name){}
};

struct reflector
{ 
    // Get the number of fields
    template<class T>
    struct fields
    {
        static const size_t n = T::annotations::__fcount;
    };

    //Get field_data at index N
    template<class T>
    static typename T::annotations get_entity_data()
    {
        return typename T::annotations();
    }

    //Get field_data at index N
    template<int N, class T>
    static typename T::template field_data<N, T> get_field_data(T& x)
    {
        return typename T::template field_data<N, T>(x);
    }

    template<class C, class Visitor, std::size_t...Is>
    static void visit_each(C& c, Visitor& v, std::index_sequence<Is...>)
    {
        int dummy[] = {0, (v(reflector::get_field_data<Is>(c),
                             reflector::get_entity_data<C>(), sizeof...(Is) ), 0)...};
        static_cast<void>(dummy);
    }

    template<class C, class Visitor>
    static void visit_each(C& c, Visitor&& v)
    {
        visit_each(c, v, std::make_index_sequence<C::annotations::__fcount>{});
    }

};

inline std::string to_string(const std::string& str)
{
    return std::string{str};
}


template <typename T> auto has_size_impl(int) -> decltype(std::declval<T>().size(), std::true_type{});
template <typename T> auto has_size_impl(...) -> std::false_type;
template <typename T>
using has_size = decltype(has_size_impl<T>(0));


/**
 * if not a float, int long char... 
 */
template<typename _Tp>
struct is_simple_type : public std::integral_constant<bool,
        std::is_fundamental<_Tp>::value
        || std::is_convertible<_Tp, std::string>::value >
{ };


template<typename _Tp>
struct is_simple_or_datatime_type : public std::integral_constant<bool,
        is_simple_type<_Tp>::value
        || std::is_convertible<_Tp, std::chrono::time_point<std::chrono::system_clock>>::value
        || std::is_enum<_Tp>::value >
{ };  

template<typename T, typename _ = void>
struct is_container : std::false_type {};

template<typename... Ts>
struct is_container_helper {};

template<typename T>
struct is_container<
        T,
        std::conditional_t<
            false,
            is_container_helper<
                typename T::value_type,
                typename T::size_type,
                typename T::allocator_type,
                typename T::iterator,
                typename T::const_iterator,
                decltype(std::declval<T>().size()),
                decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end()),
                decltype(std::declval<T>().cbegin()),
                decltype(std::declval<T>().cend())
                >,
            void
            >
        > : public std::true_type {};

struct end_string_delimit : std::ctype<char> {
    end_string_delimit() : std::ctype<char>(get_table()) {}
static mask const* get_table()
{
    static mask rc[table_size];
    rc['\0'] = std::ctype_base::space;
    return &rc[0];
}
};
static end_string_delimit* is_space = new end_string_delimit;
static std::locale delimit_endl{std::cin.getloc(), is_space};

#endif // MOR_H
