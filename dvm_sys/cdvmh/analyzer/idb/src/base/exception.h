/*! \file
    \brief Содержит базовые конструкции для обработки ошибок.
*/
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "declaration.h"
#include "utility.h"
#include "cell.h"

//! Описывает ошибку.
/*! \param name_ Название ошибки.
    \param brief_ Краткое описание ошибки.
    \param suberrors_ Подмножество ошибок, на котороые делится описываемая ошибка.
    Подмножество ошибок должно быть описано в виде `CELL_COLL_N( Error_1, ...., Error_N)`.
    \param type_ Тип ошбики (например, \c Base::Error, \c Base::Warning). 
    Если в качестве типа указан \c Utility::Null, то тип определяется по дочерней ошибке.
*/
#define ERROR_DECL( name_, brief_, suberrors_, type_) \
struct name_ \
{\
    /*! \brief Тип ошибки */ \
    typedef type_ Type; \
    /*! \brief Подмножество ошибок. */ \
    typedef suberrors_ ValueType; \
    \
    /*! \brief Краткое описание ошибки. */ \
    TO_TEXT( Brief, brief_ ) \
};

//! Определяет методы для вычисления свойств ошибки.
/*! \param id_ Тип - идентификатор ошибки.
    \param app_ Тип - приложение в котором произошла ошибка.
    Макрос должен быть включен в класс, описывающий ошибку.
    \sa \c Exception::Exception
*/
#define ERROR_ACCESS( id_, app_) \
/*! \copydoc IException::RelativeCode( ) */ \
::Base::ErrorCode RelativeCode( ) const throw( ) { return ::Base::CodeOfError< id_, app_ >( ); } \
\
/*! \copydoc IException::Brief( )*/ \
::Base::Text Brief( ) const throw( ) { return ::Base::BriefOfError< id_ >( ); }\
\
/*! \copydoc IException::Type( )*/ \
::Base::Text Type( ) const throw( ) { return ::Base::TypeOfError< id_, app_ >( ); }

//! Описывает ошибку внутри соответсвующего ей исключения.
/*! \param exception_ Исключение, соответсвующее ошибке.
    \param error_ Название ошибки.
    \param constructor_ Параметры и тело конструтктора ошибки.
    \sa \c Exception::Exception
*/
#define ERROR_DEF( exception_, error_, constructor_) \
private: \
    /*! \brief Описывает ошибку.
        \typaram Exception_ Исключение, соответсвующее ошибке.
        \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL. 
    */ \
    template< class Exception_, class ErrorId_ > \
    struct _##error_ : \
        public _##error_< Exception_, typename ::Base::ErrorTraits< ErrorId_ >::ParentId > \
    { \
        /*! Описание ошибки. */ \
        typedef ErrorId_ ErrorId; \
        \
        /*! Базовая ошибка. */ \
        typedef _##error_< Exception_, typename ::Base::ErrorTraits< ErrorId_ >::ParentId > Super; \
        \
        /*! Используемое приложение */ \
        typedef typename Super::Application Application; \
        \
        ERROR_ACCESS( ErrorId, Application) \
        \
        /*! Создает ошибку*/ \
        _##error_ constructor_ \
    }; \
    \
    /*! \brief Описывает ошибку */ \
    template< class Exception_ > struct _##error_< Exception_, Utility::Null > : \
    public Exception_ \
    { \
        /*! Описание ошибки. */ \
        typedef Utility::Null ErrorId; \
        \
        /*! Базовая ошибка. */ \
        typedef Exception_ Super; \
        \
        /*! Создает ошибку*/ \
        _##error_ constructor_ \
        \
    }; \
    \
public: \
    /*! \brief Описывает ошибку.
        \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL.
    */ \
    template< class ErrorId_ = Utility::Null > \
    struct error_ : \
        public _##error_< exception_, ErrorId_ > \
    { \
        /*! Базовая ошибка. */ \
        typedef _##error_< exception_, ErrorId_ > Super; \
        \
        /*! Идентификатор ошибки */ \
        typedef typename Super::ErrorId ErrorId; \
        \
        /*! Создает ошибку*/ \
        error_ constructor_ \
    };

//! Содержит конструкции для обработки ошибок в программе.
namespace Base
{
    typedef int ErrorCode; //!< Код возникшей ошибки.
    struct IException;

    //! Содержит базовый перечень ошибок.
    namespace ErrorList
    {
        //! Фатальная ошибка.
        struct Fatal { TO_TEXT( Brief, TEXT( "fatal error")) typedef Utility::Null ValueType; };

        //! Ошибка.
        struct Error { TO_TEXT( Brief, TEXT( "error")) typedef Utility::Null ValueType; };

        //! Предупреждение.
        struct Warning { TO_TEXT( Brief, TEXT( "warning")) typedef Utility::Null ValueType; };

        typedef CELL_COLL_3( Warning, Error, Fatal) Priority;

        //! Неизвестная ошибка.
        ERROR_DECL( Unclassified, TEXT( "unclassified error"), Utility::Null, Error)

        //! Ошибка присваивания неопределенного значения классу обертке.
        ERROR_DECL( Assign, TEXT( "can't assign undefined value to a strong value object"), Utility::Null, Error)

        //! Описывает ошибки, связанные с приведением типов.
        ERROR_DECL( TypeCast, TEXT( "type cast is not valid"), Utility::Null, Error)

        //! Разыменование нулевого указателя.
        ERROR_DECL( NullDereference, TEXT( "null pointer dereference"), Utility::Null, Fatal)

        //! Неподдерживаемая функциональность.
        ERROR_DECL( Unsupported, TEXT( "functionality is unsupported yet"), Utility::Null, Error)

        //! Ошибки при копировании строк.
        ERROR_DECL( CopyString, TEXT( "can't copy string"), Utility::Null, Error)

        //! Ошибки при преобразовании строк.
        ERROR_DECL( ConvertString, TEXT( "can't convert string"), Utility::Null, Error)
    }

    //! Общие сведения о библиотеке базовых конструкций
    struct BCL
    {
        //! Название.
        DESCRIPTION_FIELD( Title, TEXT( "Title"), TEXT( "Base construction library"))

        //! Аббревиатура.
        DESCRIPTION_FIELD( Acronym, TEXT( "Acronym"), TEXT( "BCL"))

        //! Версия.
        DESCRIPTION_FIELD( Version, TEXT( "Version"), TEXT( "3.0beta1.0, 02.02.2014"))

        //! Автор.
        DESCRIPTION_FIELD( Author, TEXT( "Author"), TEXT( "Nikita A. Kataev, kataev_nik@mail.ru"))

        //! Описание.
        typedef CELL_COLL_4( Title, Acronym, Version, Author) Description;

        //! Перечень возможных ошибок.
        typedef CELL_COLL_7( ErrorList::Assign, ErrorList::TypeCast, 
                             ErrorList::NullDereference, ErrorList::Unclassified, 
                             ErrorList::Unsupported, ErrorList::CopyString, ErrorList::ConvertString) Errors;

        //! Приоритет ошибок.
        typedef Base::ErrorList::Priority ErrorPriority; 

        //! Перечень используемых приложений.
        typedef Utility::Null Applications;

        //! Заглушка для использования статических списков приложений.
        typedef Utility::Null ValueType;
    };

    //! Возвращает количество ошибок, определяемых заданным описанием ошибки.
    /*! \tparam ErrorDecl_ Ошибка, описанная с помощью макроса \c ERROR_DECL.
    */
    template< class ErrorDecl_ > inline ErrorCode CountOfErrors( );

    //! Возвращает количество ошибок, определяемых приложением.
    /*! Количество ошибок определяется без учета используемых дополнительных приложений.
        \tparam Application_ Приложение.
    */
    template< class Application_ > inline ErrorCode CountOfAppErrors( );

    //! Возвращает код ошибки.
    /*! \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL.
        \tparam Application_ Приложение, в котором произошла ошибка.
    */
    template< class ErrorId_, class Application_ > inline ErrorCode CodeOfError( );

    //! Вычисляет смещение кода ошибки для заданного приложения.
    /*! \tparam Application_ Приложение, в котором произошла ошибка.
        \param [in] e Исключение, описывающее ошибку.
    */
    template< class Application_ > inline ErrorCode OffsetOfError( const IException *e);

    //! Возвращает тип ошибки.
    /*! \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL.
        \tparam Application_ Приложение, в котором произошла ошибка.
    */
     template< class ErrorId_, class Application_ > inline Base::Text TypeOfError( );

    //! Возвращает описание ошибки.
    /*! \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL.
    */
    template< class ErrorId_ > inline Base::Text BriefOfError( );

    //! Возвращает текстовое представление ошибки.
    /*! \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL.
        \tparam Application_ Приложение, в котором произошла ошибка.
    */
    template< class ErrorId_, class Application_ >
    inline Text ErrorToText( )
    {
        return Application_::Acronym::Data( ) + TEXT( ": ")+
               TypeOfError< ErrorId_, Application_>( ) + TEXT( " ") + Base::ToText( CodeOfError< ErrorId_, Application_ >( )) + TEXT( ": ") +
               BriefOfError< ErrorId_ >( );
    }

    //! Определяет базовою ошибку для данной.
    /*! Для ошибки `CELL_COLL_3( Error, SubError, SubSubError)`, 
        базовой будет ошибка `CELL_COLL_3( Error, SubError)`.
        \tparam ErrorId_ Идентификатор ошибки, заданный
        в виде `CELL_COLL_N( Error, SubError, SubSubError, ...)`.
        Все ошибки, образующие данную ошибку должны быть описаны с помощью макроса \c ERROR_DECL.
    */
    template< class ErrorId_ > struct ErrorTraits
    {
        typedef ErrorId_ ErrorId; //!< Идентификатор ошибки.
        typedef typename Base::ReverseCell< 
            typename Base::ReverseCell< ErrorId_ >::Result::CellNext >::Result ParentId; //!< Родительская ошибка.
    };

    //! Интерфейсный класс для исключений.
    struct IException
    {
         //! \name Access-методы
        //@{
        //! Возвращает абсолютный код ошибки в заданном приложения.
        /*! \tparam Application_ Приложение, относительно которого вычисляется код ошибки.
            \pre Ошибка, описываемая исключением, 
            должна быть указаны в перечне ошибок приложнеия \c Application_,
            либо в перечне ошибок одного из используемых внутри приложения приложений,
            определяемых через \c Application_::Applications.
            \par Пример
            Пусть при копировании строк с помощью метода \c Base::CopyString( ) 
            произошла ошибка \c Base::ErrorList::CopyString.
            Данная ошибка относится к ошибкам приложения \c BCL, 
            описываемого пространствами имен \c Base и \c Utility.
            Код ошибки в приложении \c Base::BCL равен \c 6, это ее относительный кода, 
            который будет возвращен методом RelativeCode( ).
            Приложение \c Base::BCL используется внутри приложения \с DataBase::IDB,
            поэтому абсолютный код данной ошибки в приложении \c DataBase::IDB,
            будет другим: он будет пересчитан с учетом всех ошибок,
            возможных в приложении \c DataBase::IDB и всех используемых им приложений.
            Абсолютный код можно получить с помощью метода \c Code( ), 
            указав параметр шаблона \c DataBase::IDB: \c Code< IDB >( ).
            В рамках одного приложения разные ошибки имеют разный абсолютный код,
            относительные коды могут совпадать.
        */
        template< class Application_ > ErrorCode Code( ) const
        {
            return OffsetOfError< Application_ >( this) + RelativeCode( );
        }

        //! Возвращает текстовое описание ошибки.
        /*! \tparam Application_ Используемое приложение.
            \pre Ошибка, описываемая исключением, 
            должна быть указаны в перечне ошибок приложнеия \c Application_,
            либо в перечне ошибок одного из используемых внутри приложения приложений,
            определяемых через \c Application_::Applications.
        */
        template< class Application_ > Base::Text ToText( ) const
        {
            return Application_::Acronym::Data( ) + TEXT( ": ") +
                   Type( ) + TEXT( " ") + Base::ToText( Code< Application_ >( )) + TEXT( ": ") +
                   Message( );
        }

        //! Возвращает относительный код возникшей ошибки.
        /*! Возвращает код ошибки, в приложении, непосредственно выбросившем ошибку.
            При этом не учитывается частью каких приложений является данное приложение.
            \par Пример
            Пусть при копировании строк с помощью метода \c Base::CopyString( ) 
            произошла ошибка \c Base::ErrorList::CopyString.
            Данная ошибка относится к ошибкам приложения \c BCL, 
            описываемого пространствами имен \c Base и \c Utility.
            Код ошибки в приложении \c Base::BCL равен \c 6, это ее относительный кода, 
            который будет возвращен методом \c RelativeCode( ).
            Приложение \c Base::BCL используется внутри приложения \с DataBase::IDB,
            поэтому абсолютный код данной ошибки в приложении \c DataBase::IDB,
            будет другим: он будет пересчитан с учетом всех ошибок,
            возможных в приложении \c DataBase::IDB и всех используемых им приложений.
            Абсолютный код можно получить с помощью метода \c Code( ), 
            указав параметр шаблона \c DataBase::IDB: \c Code< IDB >( ).
            В рамках одного приложения разные ошибки имеют разный абсолютный код,
            относительные коды могут совпадать.
        */
        virtual ErrorCode RelativeCode ( ) const throw( ) = 0;

        //! Возвращает краткое описание ошибки.
        virtual Base::Text Brief( ) const throw( ) = 0;

        //! Возвращает полное описание описание ошибки.
        virtual Base::Text Message( ) const throw( ) { return Brief( ); }

        //! Возвращает тип ошибки.
        virtual Base::Text Type( ) const throw( ) = 0;
        //@}
    };

    //! Базовый класс-исключение описывающее ошибку.
    /*! \tparam Application_ Приложение, в котором произошла ошибка.
    */
    template< class Application_ >
    struct Exception : public IException
    {
        typedef Exception< Application_ > This; //!< Данное исключение.
        typedef Base::ErrorCode ErrorCode; //!< Код возникшей ошибки.
        typedef Application_ Application; //!< Приложение в котором возникла ошибка.

        //! \name Описание ошибок.
        //@{
        ERROR_DEF( This, Error, ( ) throw( ) { } )

        //! Описывает ошибку, недопускающую получения имени базы данных по дескриптору.
        template< class ErrorId_ > struct TypeCastError : 
            public Error< ErrorId_ >
        {
        public:
            /*! \copydoc Exception::Exception( )
                \param [in] fromType Тип, из которого выполняется преобразование.
                \param [in] toType Тип, в который выполняется преобразование.
             */
            TypeCastError( const Text &fromType, const Text &toType) throw ( )
                : Error< ErrorId_ >( ), m_fromType( fromType), m_toType( toType) {  }

            //! Возвращает тип, из которого выполняется преобразование.
            const Text & FromType( ) const throw( ) { return m_fromType;}

            //! Возвращает тип, в который выполняется преобразование.
            const Text & ToType( ) const throw( ) { return m_toType;}

            virtual Base::Text Message( ) const throw( )
            {
                return BriefOfParent< typename ErrorTraits< ErrorId_ >::ParentId >( 
                            Utility::IsIdentical< typename ErrorTraits< ErrorId_ >::ParentId, Utility::Null >( )) + 
                       TEXT( "type cast from type '") + m_fromType +
                       TEXT( "' to type '") + m_toType + TEXT( "' is not valid");
            }
        private:
            //! Возвращает описание родительской ошибки.
            template< class ParentId_ > Text BriefOfParent( Utility::False) const throw( )
            {
                return BriefOfError< ParentId_ >( ) + TEXT( ": ");
            }

            //! Возвращает описание родительской ошибки.
            template< class ParentId_ > Text BriefOfParent( Utility::True) const throw( )
            {
                return TEXT("");
            }

        private:
            Text m_fromType; //!< Тип, из которого выполняется преобразование.
            Text m_toType; //!< Тип, в который выполняется преобразование.
        };
        //@}

        //! Разрушает исключение.
        virtual ~Exception( ) { }
        //@}
    };

    namespace Detail
    {
        //! Возвращает количество ошибок в дереве с заданным корнем.
        template< class Head_ > ErrorCode ErrorCountInHead( );

        //! Возвращает количество ошибок, определяемых заданным списком ошибок.
        /*! \tparam ErrorList_ Описание ошибок с помощью коллекции ячеек, 
            каждая ячейка в которой описывает ошибку с помощью макроса \c ERROR_DECL.
        */
        template< class ErrorList_ > inline ErrorCode ErrorCountInList( );

        template< class ErrorList_ > inline ErrorCode ErrorCountInList( )
        {
            typedef typename ErrorList_::CellId Head;
            typedef typename ErrorList_::CellNext Tail;

            return ErrorCountInHead< Head >( ) + ErrorCountInList< Tail >( );
        }

        //! Перегрузка для случая, когда обработан весь список ошибок.
        template< > inline ErrorCode ErrorCountInList< Utility::Null >( ) 
        {
            return 0;
        }

        template< class Head_ > ErrorCode ErrorCountInHead( )
        {
            typedef typename Head_::ValueType HeadList;
            return ErrorCountInList< HeadList >( ) + 1;
        }

        //! Вычисляет код ошибки.
        template< class Tail_, class ErrorList_ >
        struct CodeOfErrorImp
        {
            //! Вычисляет код ошибки.
            static ErrorCode Evaluate( )
            {
                typedef typename Tail_::CellId Head;
                typedef typename ErrorList_::CellId Current;

                return Evaluate( Utility::IsIdentical< Head, Current >( )) + 1;
            }

        private:
            //! Вычисляет код ошибки.
            static ErrorCode Evaluate( Utility::True)
            {
               return  CodeOfErrorImp< typename Tail_::CellNext, 
                                       typename Tail_::CellId::ValueType >::Evaluate( );
            }

            //! Вычисляет код ошибки.
            static ErrorCode Evaluate( Utility::False)
            {
                return CountOfErrors< typename ErrorList_::CellId >( ) + 
                       CodeOfErrorImp< Tail_, typename ErrorList_::CellNext >::Evaluate( );
            }
        };

        //! Вычисляет код ошибки.
        template< class ErrorList_ >
        struct CodeOfErrorImp< Utility::Null, ErrorList_ >
        {
            //! Вычисляет код ошибки.
            static ErrorCode Evaluate( ) { return 1; }
        };

        //! Определяет тип ошибки.
        template< class ErrorId_, class Priority_ >
        struct TypeOfErrorImp
        {
            typedef typename ErrorId_::CellId::Type CurrentType;
            typedef typename Base::IsCellExist< Priority_, CurrentType > NewPriority;
            typedef typename Utility::If< typename NewPriority::Definition,
                                           TypeOfErrorImp< typename ErrorId_::CellNext, 
                                                                   typename NewPriority::Cell >,
                                           TypeOfErrorImp< typename ErrorId_::CellNext, 
                                                                   Priority_ > >::Result::Type Type;
        };

        //! Определяет тип ошибки.
        template< class Priority_ > 
        struct TypeOfErrorImp< Utility::Null, Priority_ >
        {
            typedef typename Priority_::CellId Type;
        };

        //! Объекдиняет описание всех ячеек в одно.
        struct MergeBriefFunctor
        {
             //! Объекдиняет описание всех ячеек в одно.
            template< class Cell_ > void operator( )( )
            {
                typedef typename Cell_::CellId CellId;
                typedef typename Cell_::CellNext CellNext;

                m_brief = m_brief + CellId::Brief( );

                Utility::AddToObjectIf< Base::Text >( 
                    Utility::Not< Utility::IsIdentical< CellNext, Utility::Null > >( ), 
                    m_brief, ": ");
            }

            //! Возвращает объединенное описание.
            Base::Text Brief( ) const { return m_brief; }

        public:
            Base::Text m_brief; //!< Объединенное описание.
        };

        //! Вычисляет смещение кода ошибки для заданного приложения.
        template< class Application_ > inline std::pair< ErrorCode, bool> OffsetOfError( const IException *e);

        //! Вычисляет смещение кода ошибки для заданного приложения.
        template< class ApplicationList_ > inline std::pair< ErrorCode, bool> OffsetOfErrorInList( const IException *e);

        template< class Application_ > 
        inline std::pair< ErrorCode, bool> OffsetOfError( const IException *e)
        {
            typedef std::pair< ErrorCode, bool> Result;
            const Exception< Application_ > *eApp = dynamic_cast< const Exception< Application_ > * >( e);
            if ( eApp)
                return Result( 0, true);

            Result offset = OffsetOfErrorInList< typename Application_::Applications >( e);
            return Result( ErrorCountInList< typename Application_::Errors >( ) + offset.first,
                           offset.second);
        }

        template< class ApplicationList_ > 
        inline std::pair< ErrorCode, bool> OffsetOfErrorInList( const IException *e)
        {
            typedef std::pair< ErrorCode, bool> Result;

            Result offset = Detail::OffsetOfError< typename ApplicationList_::CellId>( e);
            if ( offset.second)
                return offset;

            Result nextOffset = OffsetOfErrorInList< typename ApplicationList_::CellNext >( e);
            return Result( offset.first + nextOffset.first, nextOffset.second);
        }

        //! Вычисляет смещение кода ошибки для заданного приложения.
        template< > inline std::pair< ErrorCode, bool> OffsetOfErrorInList< Utility::Null >( const IException *) 
        {
            return std::make_pair( 0, false);
        }
    }

    template< class ErrorDecl_ > inline ErrorCode CountOfErrors( )
    {
        return Detail::ErrorCountInList< typename ErrorDecl_::ValueType >( );
    }

    template< class ErrorId_, class Application_ > inline ErrorCode CodeOfError( )
    {
        return Detail::CodeOfErrorImp< ErrorId_, 
                                       typename Application_::Errors >::Evaluate( ) - 1;
    }

    template< class ErrorId_ > inline Base::Text BriefOfError( )
    {
        Detail::MergeBriefFunctor briefFunctor;
        ErrorId_::ForeachDefinition( briefFunctor);

        return briefFunctor.Brief( );
    }

    template< class ErrorId_, class Application_ > inline Base::Text TypeOfError( )
    {
        return Detail::TypeOfErrorImp< ErrorId_, 
                                       typename Application_::ErrorPriority >::Type::Brief( );
    }

    template< class Application_ > inline ErrorCode OffsetOfError( const IException *e)
    {
        typedef std::pair< ErrorCode, bool> Result;

        Result offset = Detail::OffsetOfError< Application_ >( e);
        ASSERT( offset.second);

        return offset.first;
    }
}

#endif//EXCEPTION_H
