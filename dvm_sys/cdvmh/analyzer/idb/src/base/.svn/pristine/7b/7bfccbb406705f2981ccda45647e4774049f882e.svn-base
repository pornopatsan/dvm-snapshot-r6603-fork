/*! \file
    \brief Содержит "прозрачные" классы-обретки для типов.

    Позволяет задавать неопределенные значения для объектов разных типов.
*/
#ifndef VALUE_H
#define VALUE_H

#include "declaration.h"
#include "utility.h"
#include "exception.h"
#include <ostream>

namespace Utility
{
    using DataBase::Text;
    using DataBase::Char;

    //! Класс-обертка для значений заданного типа.
    /*! \tparam Type_ Тип значения.
        \tparam Nullable_ Признак допустимости неопределенного значения.
        \attention Для типа \c Type_ Должны быть определены конструктор умолчания, копирования
        и опретор присваивания.
    */
    template< class Type_, class Nullable_ > class Value;

    //! Класс-обертка запрещающая создание указателей, недопускающих неопределенное значения.
    /*! \tparam Type_ Тип значения.
    */
    template< class Type_ > class Value< Type_ *, Utility::False >;

    //! Класс-обертка для значений, недопускающих неопределенного значения.
    /*! \tparam Type_ Тип значения.
    */
    template< class Type_ > 
    class Value< Type_, Utility::False >
    {
    public:
        typedef Type_ Type; //!< Тип значения.
        typedef Utility::False Nullable; //!< Допустимость неопределенного значения.

        typedef Value< Type, Utility::False > Strong; //!< Класс, недопускающий неопределенные значения.
        typedef Value< Type, Utility::True > Weak; //!< Класс, допускающий неопределенные значения.       

    public:
        //! Конструктор умолчания.
        Value ( ) { }

        //! Конструктор копирования на основе хранимого значения.
        Value( const Type &value) : 
            m_value( value) { }

        //! Конструктор копирования на осонве класса, недопускающего неопределенные значения.
        Value( const Strong &value) : 
            m_value( value) { }

        //! Конструктор копирования на осонве класса, допускающего неопределенные значения.
        Value( const Weak &value) { *this = value; }

        //! Оператор присваивани на основе хранимого значения.
        Strong & operator=( const Type &value) 
        {
            m_value = value;
            return *this;
        }

        //! Оператор присваивания на осонве класса, недопускающего неопределенные значения.
        Strong & operator=( const Strong &value) 
        {
            m_value = value;
            return *this;
        }

        //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Strong & operator=( const Weak &value) 
        {
            if ( value == undef)
                throw Base::Exception< Base::BCL >::Error< CELL_COLL_1( Base::ErrorList::Assign) >( );
            m_value = value;
            return *this;
        }       

        //! Оператор преобразования к типу значения.
        operator Type & ( ) { return m_value; }

        //! Оператор преобразования к типу значения.
        operator const Type & ( ) const { return m_value; }              

        //! Конструктор копирования, неопределенного значения.
        bool operator==( Null) { return false; }      

        //! Конструктор копирования, неопределенного значения.
        bool operator==( Null) const { return false; }      

        //! Конструктор копирования, неопределенного значения.
        bool operator!=( Null) { return true; }       

        //! Конструктор копирования, неопределенного значения.
        bool operator!=( Null) const { return true; }       
        
    private:
        Type m_value; //!< Значение.
    };

    //! Класс-обертка для скалярных типов считается скалярным типом.
    template< class Type_ > struct IsScalar< Value< Type_, Utility::False > > : 
        public IsScalar< Type_ > { };

    //! Класс-обертка для значений, допускающих неопределенного значения.
    /*! \tparam Type_ Тип значения.
    */
    template< class Type_ > 
    class Value< Type_, Utility::True >
    {
    public:
        typedef Type_ Type; //!< Тип значения.
        typedef Utility::True Nullable; //!< Допустимость неопределенного значения.

        typedef Value< Type, Utility::False > Strong; //!< Класс, недопускающий неопределенные значения.
        typedef Value< Type, Utility::True > Weak; //!< Класс, допускающий неопределенные значения.       

    public:
        //! Конструктор умолчания, создает неопределенное значение.
        Value( ) : m_defined( false){ }

        //! Конструктор копирования, неопределенного значения.
        Value( Null) : m_defined( false) { }        

        //! Конструктор копирования на основе хранимого значения.
        Value( const Type &value) : 
            m_value( value), 
            m_defined( true) { }

        //! Конструктор копирования на осонве класса, недопускающего неопределенные значения.
        Value( const Strong &value) : 
            m_value( value), 
            m_defined( true) { }

        //! Конструктор копирования на осонве класса, допускающего неопределенные значения.
        Value( const Weak &value) : 
            m_value( value.m_value), 
            m_defined( value.m_defined)  { }

        //! Конструктор копирования, неопределенного значения.
        Weak & operator=( Null) 
        {            
            m_defined = false;             
            return *this;
        }      

        //! Оператор присваивани на основе хранимого значения.
        Weak & operator=( const Type &value) 
        {
            m_value = value;
            m_defined = true;
            return *this;
        }

        //! Оператор присваивания на осонве класса, недопускающего неопределенные значения.
        Weak & operator=( const Strong &value) 
        {
            m_value = value;
            m_defined = true;
            return *this;
        }

         //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Weak & operator=( const Weak &value) 
        {           
            m_value = value.m_value;
            m_defined = value.m_defined;
            return *this;
        }       

        //! Оператор преобразования к типу значения.
        operator Type & ( ) { return m_value; }

        //! Оператор преобразования к типу значения.
        operator const Type & ( ) const { return m_value; }        

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) { return !m_defined; }      

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) const { return !m_defined; }      

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null) { return m_defined; }

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null) const { return m_defined; }
        
    private:
        Type m_value; //!< Значение.
        bool m_defined; //!< Признак неопределенности значения.
    };

    //! Класс-обертка для скалярных типов считается скалярным типом.
    template< class Type_ > struct IsScalar< Value< Type_, Utility::True > > : 
        public IsScalar< Type_ > { };

    //! Класс-обертка для указателей, допускающих неопределенного значения.
    /*! \tparam Type_ Тип значения.
    */
    template< class Type_ > 
    class Value< Type_ *, Utility::True >
    {
    public:
        typedef Type_ * Type; //!< Тип значения.
        typedef Utility::True Nullable; //!< Допустимость неопределенного значения.

        typedef Value< Type, Utility::False > Strong; //!< Класс, недопускающий неопределенные значения.
        typedef Value< Type, Utility::True > Weak; //!< Класс, допускающий неопределенные значения.       

    public:
        //! Конструктор умолчания, создает неопределенное значение.
        Value( ) : m_value( NULL) { }

        //! Конструктор копирования, неопределенного значения.
        Value( Null) : m_value( NULL) { }

        //! Конструктор копирования на основе хранимого значения.
        Value( const Type &value) : 
            m_value( value) { }        

        //! Конструктор копирования на осонве класса, допускающего неопределенные значения.
        Value( const Weak &value) : 
            m_value( value.m_value) { }

        //! Конструктор копирования, неопределенного значения.
        Weak & operator=( Null) 
        { 
            m_value = NULL;   
            return *this;
        }      

        //! Оператор присваивани на основе хранимого значения.
        Weak & operator=( const Type &value) 
        {
            m_value = value;
            return *this;
        }       

         //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Weak & operator=( const Weak &value) 
        {           
            m_value = value.m_value;
            return *this;
        }       

        //! Оператор преобразования к типу значения.
        operator Type & ( ) { return m_value; }

        //! Оператор преобразования к типу значения.
        operator const Type & ( ) const { return m_value; }        

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) { return m_value; }      

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) const { return m_value; }      

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null) { return m_value; }      

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null) const { return m_value; }  
        
    private:
        Type m_value; //!< Значение.
    };

    //! Класс-обертка для указателем считается указателем.
    template< class Type_ > struct IsPointer< Value< Type_, Utility::False > > 
        : public IsPointer< Type_ > { };

    //! Класс-обертка для указателем считается указателем.
    template< class Type_ > struct IsPointer< Value< Type_, Utility::True > > 
        : public IsPointer< Type_ > { };

    //! Класс-обертка для строк типа \c Text, допускающих неопределенное значение.
    /*! \tparam Nullable_ Признак допустимости неопределенного значения.
    */
    template< > 
    class Value< Text, Utility::True >
        : public Text
    {
    public:
        typedef Text Type; //!< Тип значения.
        typedef Utility::True Nullable; //!< Допустимость неопределенного значения.

        typedef Value< Type, Utility::False > Strong; //!< Класс, недопускающий неопределенные значения.
        typedef Value< Type, Utility::True > Weak; //!< Класс, допускающий неопределенные значения.       

    public:
        //! Конструктор умолчания, создает неопределенное значение.
        Value( ) : m_defined( false){ }

        //! Конструктор копирования, неопределенного значения.
        Value( Null) : m_defined( false) { }

        //! Конструктор копирования на основе хранимого значения.
        Value( const Text &value) : 
            Text( value),
            m_defined( true) { }

        //! Конструктор копирования на основе хранимого значения.
        Value( const Char *value) : 
            Text( value),
            m_defined( true) { }  

        //! Конструктор копирования на осонве класса, допускающего неопределенные значения.
        Value( const Weak &value) : 
            Text( value),
            m_defined( value != undef)
        { }

        //! Конструктор копирования на осонве класса, недопускающего неопределенные значения.
        Value( const Strong &value);

        //! Конструктор копирования, неопределенного значения.
        Weak & operator=( Null) 
        { 
            m_defined = false;   
            return *this;
        }

        //! Оператор присваивани на основе хранимого значения.
        Weak & operator=( const Type &value) 
        {
            *(static_cast< Text * >(this)) = value;
            m_defined = true;
            return *this;
        }

        //! Оператор присваивани на основе хранимого значения.
        Weak & operator=( const Char *value)
        {
            *(static_cast< Text * >(this)) = value;
            m_defined = true;
            return *this;
        }

         //! Оператор присваивани на основе хранимого значения.
        Weak & operator=( Char value) 
        {
            *(static_cast< Text * >(this)) = value;
            m_defined = true;
            return *this;
        }

         //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Weak & operator=( const Weak &value)
        {
            *(static_cast< Text * >(this)) = value;
            m_defined = value != undef;
            return *this;
        }

        //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Weak & operator=( const Strong &value);

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) { return !m_defined; }

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) const { return !m_defined; }

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null)  { return m_defined; }

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null) const { return m_defined; }
     
    private:
        bool m_defined; //!< Признак неопределенного значения.
    };

    //! Оператор сравнения двух классов-оберток.
    /*! Выполняет сравнение на равенство двух классов-оберток, 
        допускающих неопределенные значения.
    */
    template< class Left_, class Right_ > 
    inline bool operator==( const Value< Left_, Utility::True > &left, 
                            const Value< Right_, Utility::True > &right)
    {
        return (left == undef && right == undef) ||
               (left != undef && right != undef &&
                static_cast< const Left_ & >( left) == static_cast< const Right_ & >( right));
    }

    //! Оператор сравнения двух классов-оберток.
    /*! Выполняет сравнение на неравенство двух классов-оберток, 
        допускающих неопределенные значения.
    */
    template< class Left_, class Right_ > 
    inline bool operator!=( const Value< Left_, Utility::True > &left, 
                            const Value< Right_, Utility::True > &right)
    {
        return !( left == right);
    }

    //! Оператор сравнения двух классов-оберток.
    /*! Выполняет сравнение на меньше двух классов-оберток, 
        допускающих неопределенные значения.
    */
    template< class Left_, class Right_ > 
    inline bool operator<( const Value< Left_, Utility::True > &left, 
                            const Value< Right_, Utility::True > &right)
    {
        if ( left == undef || right == undef)
                return false;
        return static_cast< const Left_ & >( left) < static_cast< const Right_ & >( right);
    }

    //! Оператор сравнения двух классов-оберток.
    /*! Выполняет сравнение на меньше-равно двух классов-оберток, 
        допускающих неопределенные значения.
    */
    template< class Left_, class Right_ > 
    inline bool operator<=( const Value< Left_, Utility::True > &left, 
                            const Value< Right_, Utility::True > &right)
    {
        if ( left == undef || right == undef)
                return false;
        return static_cast< const Left_ & >( left) < static_cast< const Right_ & >( right);
    }
    //! Оператор сравнения двух классов-оберток.
    /*! Выполняет сравнение на больше двух классов-оберток, 
        допускающих неопределенные значения.
    */
    template< class Left_, class Right_ > 
    inline bool operator>( const Value< Left_, Utility::True > &left, 
                            const Value< Right_, Utility::True > &right)
    {
        if ( left == undef || right == undef)
                return false;
        return static_cast< const Left_ & >( left) > static_cast< const Right_ & >( right);
    }

    //! Оператор сравнения двух классов-оберток.
    /*! Выполняет сравнение на больше-равно двух классов-оберток, 
        допускающих неопределенные значения.
    */
    template< class Left_, class Right_ > 
    inline bool operator>=( const Value< Left_, Utility::True > &left, 
                            const Value< Right_, Utility::True > &right)
    {
        if ( left == undef || right == undef)
                return false;
        return static_cast< const Left_ & >( left) >= static_cast< const Right_ & >( right);
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на равенство класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator==( const Left_ &left, const Value< Right_, Utility::True > &right)
    {
        return right != undef &&
               left == static_cast< const Right_ & >( right);
    }

     //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на неравенство класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator!=( const Left_ &left, const Value< Right_, Utility::True > &right)
    {
       return !( left == right);
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на меньше класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator<( const Left_ &left, const Value< Right_, Utility::True > &right)
    {
        if ( right == undef)
                return false;
        return left < static_cast< const Right_ & >( right);
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на меньше-равно класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator<=( const Left_ &left, const Value< Right_, Utility::True > &right)
    {
        if ( right == undef)
                return false;
        return left <= static_cast< const Right_ & >( right);
    }
    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на больше класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator>( const Left_ &left, const Value< Right_, Utility::True > &right)
    {
        if ( right == undef)
                return false;
        return left > static_cast< const Right_ & >( right);
    }
    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на боьлше-равно класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator>=( const Left_ &left, const Value< Right_, Utility::True > &right)
    {
        if ( right == undef)
                return false;
        return left >= static_cast< const Right_ & >( right);
    }

    //! Оператор сравнения класса-обертоки со значением.
    template< class Left_, class Right_ > 
    inline bool operator==( const Value< Left_, Utility::True > &left, const Right_ &right)
    {
        return left != undef &&
               static_cast< const Left_ & >( left) == right;
    }

    //! Оператор сравнения класса-обертоки со значением.
    template< class Left_, class Right_ > 
    inline bool operator!=( const Value< Left_, Utility::True > &left, const Right_ &right)
    {
        return !( left == right);
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на меньше класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator<( const Value< Left_, Utility::True > &left, const Right_ &right)
    {
        if ( left == undef)
                return false;
        static_cast< const Left_ & >( left) < right;
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на меньше-равно класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator<=( const Value< Left_, Utility::True > &left, const Right_ &right)
    {
        if ( left == undef)
                return false;
        static_cast< const Left_ & >( left) <= right;
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на больше-равно класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator>( const Value< Left_, Utility::True > &left, const Right_ &right)
    {
        if ( left == undef)
                return false;
        static_cast< const Left_ & >( left) > right;
    }

    //! Оператор сравнения класса-обертоки со значением.
    /*! Выполняет сравнение на больше-равно класса-обертки,
        допускающего неопределенное значение,
        с классом-оберткой, недопускающим неопределенное значение,
        либо со значением.
    */
    template< class Left_, class Right_ > 
    inline bool operator>=( const Value< Left_, Utility::True > &left, const Right_ &right)
    {
        if ( left == undef)
                return false;
        static_cast< const Left_ & >( left) >= right;
    }

    //! Оператор сравнения с неопределенным значением.    
    inline bool operator==( Null, Null) { return true; }

    //! Оператор сравнения с неопределенным значением.    
    inline bool operator!=( Null, Null) { return false; }

    //! Оператор сравнения с неопределенным значением.
    template< class Right_ > 
    inline bool operator==( Null, const Value< Right_, Utility::True > &right) { return right == undef; }

    //! Оператор сравнения с неопределенным значением.
    template< class Right_ > 
    inline bool operator!=( Null, const Value< Right_, Utility::True > &right) { return !(undef == right); }

    //! Оператор сравнения с неопределенным значением.
    template< class Right_ > 
    inline bool operator==( Null, const Value< Right_, Utility::False > &) { return false; }

    //! Оператор сравнения с неопределенным значением.
    template< class Right_ > 
    inline bool operator!=( Null, const Value< Right_, Utility::False > &) { return true; }

    //! Класс-обертка для строк типа \c Text, недопускающих неопределенное значение.
    /*! \tparam Nullable_ Признак допустимости неопределенного значения.
    */
    template< > 
    class Value< Text, Utility::False >
        : public Text
    {
    public:
        typedef Text Type; //!< Тип значения.
        typedef Utility::False Nullable; //!< Допустимость неопределенного значения.

        typedef Value< Type, Utility::False > Strong; //!< Класс, недопускающий неопределенные значения.
        typedef Value< Type, Utility::True > Weak; //!< Класс, допускающий неопределенные значения.       

    public:
        //! Конструктор умолчания.
        Value( ) { }        

        //! Конструктор копирования на основе хранимого значения.
        Value( const Text &value) : 
            Text( value) { }        

        //! Конструктор копирования на основе хранимого значения.
        Value( const Char *value) : 
            Text( value) { }        

        //! Конструктор копирования на осонве класса, допускающего неопределенные значения.
        Value( const Weak &value) { *this = value; }

        //! Конструктор копирования на осонве класса, недопускающего неопределенные значения.
        Value( const Strong &value) : 
            Text( value) { }        

        //! Оператор присваивани на основе хранимого значения.
        Strong & operator=( const Type &value) 
        {
            *(static_cast< Text * >(this)) = value;
            return *this;
        }

        //! Оператор присваивани на основе хранимого значения.
        Strong & operator=( const Char *value) 
        {
            *(static_cast< Text * >(this)) = value;
            return *this;
        }     

         //! Оператор присваивани на основе хранимого значения.
        Strong & operator=( Char*value) 
        {
            *(static_cast< Text * >(this)) = value;
            return *this;
        }      
            
        //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Strong & operator=( const Weak &value) 
        {
            if ( value == undef)
                throw Base::Exception< Base::BCL >::Error< CELL_COLL_1( Base::ErrorList::Assign) >( );

            *(static_cast< Text * >(this)) = value;
            return *this;
        }               

        //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
        Strong & operator=( const Strong &value) 
        {           
            *(static_cast< Text * >(this)) = value;
            return *this;
        }        

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) { return false; }

        //! Оператор сравнения с неопределенным значением.
        bool operator==( Null) const { return false; }

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null)  { return true; }

        //! Оператор сравнения с неопределенным значением.
        bool operator!=( Null) const { return true; }
    };     

    //! Конструктор копирования на осонве класса, недопускающего неопределенные значения.
    inline Value< Text, Utility::True >::Value( const Strong &value) : 
        Text( value), 
        m_defined( true) { }        

    //! Оператор присваивания на осонве класса, допускающего неопределенные значения.
    inline Value< Text, Utility::True >::Weak & Value< Text, Utility::True >::operator=( const Strong &value) 
    {           
        *(static_cast< Text * >(this)) = value;
        m_defined = true;
        return *this;
    }        

    //! Оператор вывода величин, допускающих неопределенные значения.
    template< class Type_ >
    std::ostream & operator<<( std::ostream & os, const Value< Type_, Utility::True > &value)
    {
        if ( value == undef)
            os << "undef";
        else
            os << static_cast< const Type_ & >( value);
        return os;
    }
}
#endif//VALUE_H
