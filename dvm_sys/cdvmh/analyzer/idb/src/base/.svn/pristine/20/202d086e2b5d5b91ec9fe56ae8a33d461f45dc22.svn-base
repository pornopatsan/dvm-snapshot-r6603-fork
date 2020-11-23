/*! \file
	\brief Содержит классы, реализующие статические коллекции ячеек.
*/
#ifndef CELL_H
#define CELL_H

#include "declaration.h"
#include "utility.h"
#include "cell_macros.h"

namespace Base
{
	//! Определяет новую ячейку в начале последовательности ячеек.
	/*! Класс предназначен для статического определения коллекций, образованных последовательностью ячеек.
		При определении ячейки указывается ее идентификатор и тип хранимого значения.
		Ячейка добавляется в начало коллекции с помощью механизма наследования. 
		Каждая ячейка наследуется от следующей за ней в коллекции ячейки.
		Последняя ячейка наследуется от неопределенной ячейки заданной классом \c Utility::Null.
		\attention Идентификатор ячейки - структура данных, внутри которой должны быть определен
        тип \c ValueType задающий тип значений хранимых в ячейке;
		\tparam CellId_ Идентификатор ячейки, используется для доступа к ней.
		\tparam CellNext_ Следующая ячейка в коллекции.
		\todo Добавить проверку того, что столбец встречается только один раз.
		\sa \ref cell_test
	*/
	template< class CellId_, class CellNext_ = Utility::Null > 
	class Cell: public CellNext_
	{         
	public:
		typedef CellId_ CellId; //!< Идентификатор ячейки, используется для доступа к ней.
		typedef typename CellId::ValueType ValueType; //!< Тип значения, хранимого в ячейке.   		
		typedef CellNext_ CellNext; //!< Следующая ячейка в коллекции.
        
        typedef ValueType & Reference; //!< Ссылка на значение.
        typedef const ValueType & ReferenceC; //!< Ссылка на значение.

	public:        
		//! \name Service-методы
		//@{					
		//! \copybrief Foreach( Functor_ &)
		/*!	Последовательно предоставляет заданному функционалу доступ к описаниям всех ячеек в коллекции.			
			\attention В вызываемом функционале должен быть определен метод 
			`template< class Cell_ > operator( )( )` (в качестве примера см. \c ColumnTextListFunctor).
			\par Алгоритм
			Фунцкионал вызывается для данной ячейки, если ячейка не последняя в коллекции,
			то выполняется переход к обработке следующей ячейки.
			\todo Cделать реалзацию с возможностью задания списка функторов.
			\todo Убрать _WIN32
			\sa \ref cell_foreach_definition "Пример"
		*/			
		template< class Functor_ > static void ForeachDefinition( Functor_ &functor)
		{
#ifdef __GNUC__
			functor.template operator( )< Cell >( );
#else
      functor.operator( )< Cell >();
#endif
			ForeachDefinition< Functor_ >( functor, Utility::IsIdentical< CellNext, Utility::Null >( ));
		} 		
		//@}				

	public:    
		//! \name Access-методы
		//@{
        //! Предоставляет доступ к значению, хранимому в ячейке.
		/*! \tparam What_ Идентификатор ячейки.			
		*/
		template< class What_ > 
        typename What_::ValueType & Value( ) 
		{	            
			return const_cast<  typename What_::ValueType & >( AccessValue< What_ >( ));
		}   

        //! Возвращает значение, хранимое в ячейке.
		/*! \tparam What_ Идентификатор ячейки.			
		*/
		template< class What_ > 
        const typename What_::ValueType & Value( ) const { return AccessValue< What_ >( ); }    				

        //! Предоставляет доступ к значению, хранимому в ячейке.
		/*! \tparam What_ Идентификатор ячейки.			
		*/
		template< class What_ > 
        typename What_::ValueType & operator[ ]( What_) { return Value< What_ >( ); }    

        //! Возвращает значение, хранимое в ячейке.
		/*! \tparam What_ Идентификатор ячейки.			
		*/
		template< class What_ > 
        const typename What_::ValueType & operator[ ]( What_) const { return Value< What_ >( ); }		
		//@}
    
		//! \name Service-методы
		//@{				
		//! Вызывает заданный функционал для каждой ячейки в коллекции.
		/*!	Последовательно предоставляет заданному функционалу доступ к значениям всех ячеек в коллекции.			
			\attention В вызываемом функционале должен быть определен метод 
			`template< class Cell_ > operator( )( Cell_ *cell)`.				
			\par Алгоритм
			Фунцкионал вызывается для данной ячейки, если ячейка не последняя в коллекции,
			то выполняется переход к обработке следующей ячейки.
			\todo Cделать реалзацию с возможностью задания списка функторов.
			\sa \ref cell_foreach "Пример"
		*/
		template< class Functor_ > void Foreach( Functor_ &functor)
		{
			functor( this);			
			Foreach< Functor_ >( functor, Utility::IsIdentical< CellNext, Utility::Null >( ));
		} 		

        //! Вызывает заданный функционал для каждой ячейки в коллекции.
		/*!	Последовательно предоставляет заданному функционалу доступ к значениям всех ячеек в коллекции.			
			\attention В вызываемом функционале должен быть определен метод 
			`template< class Cell_ > operator( )( Cell_ *cell)`.				
			\par Алгоритм
			Фунцкионал вызывается для данной ячейки, если ячейка не последняя в коллекции,
			то выполняется переход к обработке следующей ячейки.
			\todo Cделать реалзацию с возможностью задания списка функторов.
			\sa \ref cell_foreach "Пример"
		*/
		template< class Functor_ > void Foreach( Functor_ &functor) const
		{            
			functor( this);			
			Foreach< Functor_ >( functor, Utility::IsIdentical< CellNext, Utility::Null >( ));
		} 		
		//@}

	protected:
		//! Возвращает значение, хранимое в ячейке.
		/*! \tparam What_ Идентификатор ячейки.			
			\par Алгоритм 
			Ячейки рассматриваются последовательно от начала коллекции, пока не будет найдена нужная ячейка.
			Проверка того, что найдена нужная ячейка выполняется с помощью класса \c Utility::IsIdentical:
			- eсли найдена нужная ячейка, то `Utility::IsIdentical< What_, CellId >( )`
			будет объектом класса унаследованного от \c Utility::True и будет возвращено значение из ячейки с помощью метода: 
			\code template< class What_ > const typename Whate_::ValueType & AccessValue( Utility::True) const \endcode
			- иначе, `Utility::IsIdentical< What_, CellId >( )`  
			будет объектом класса унаследованного от \c Utility::False и будет рассмотрена следующая ячека в коллекции c помощью метода: 
			\code template< class What_ >const typename Whate_::ValueType & AccessValue( Utility::False) const \endcode			
		*/
		template< class What_ > 
        const typename What_::ValueType & AccessValue( ) const
		{
			return AccessValue< What_ >( Utility::IsIdentical< What_, CellId >( ));
		}

	private:
		//! Возвращает значение, хранимое в ячейке.
		template< class What_ > 
        const typename What_::ValueType & AccessValue( Utility::True) const
		{
			return m_value;
		}

		//! Переходит к рассмотрению следующей ячейки.
		template< class What_ >
        const typename What_::ValueType & AccessValue( Utility::False) const
        {
            return CellNext::template AccessValue< What_ >( );
        }
		
		//! Вызывается при переходе к обработке следующей ячейки в коллекции.		
		/*! Используется при обработке значений, хранящихся в ячейках коллекции.
		*/
		template< class Functor_ > void Foreach( Functor_ &functor, Utility::False) 
		{ 
			CellNext::template Foreach< Functor_ >( functor);
		}    

		//! Вызывается, когда обработаны всей ячейки в коллекции.
		/*! Используется при обработке значений, хранящихся в ячейках коллекции.
		*/
		template< class Functor_ > void Foreach( Functor_ &, Utility::True) { }    		

        //! Вызывается при переходе к обработке следующей ячейки в коллекции.		
		/*! Используется при обработке значений, хранящихся в ячейках коллекции.
		*/
		template< class Functor_ > void Foreach( Functor_ &functor, Utility::False) const
		{ 
			CellNext::template Foreach< Functor_ >( functor);
		}    

		//! Вызывается, когда обработаны всей ячейки в коллекции.
		/*! Используется при обработке значений, хранящихся в ячейках коллекции.
		*/
		template< class Functor_ > void Foreach( Functor_ &, Utility::True) const { }    		

		//! Вызывается при переходе к обработке следующей ячеки в коллекции.
		/*! Используется при обработке описаний ячеек в коллекции.
		*/
		template< class Functor_ > static void ForeachDefinition( Functor_ &functor, Utility::False) 
		{ 
			CellNext::template ForeachDefinition< Functor_ >( functor);
		}    

		//! Вызывается, когда обработаны все ячейки в коллекции.
		/*! Используется при обработке описаний ячеек в коллекции.
		*/
		template< class Functor_ > static void ForeachDefinition( Functor_ &, Utility::True) { }    	
    
	private:
		ValueType m_value; //!< Значение, хранимое в ячейке.   
	};

    //! Определяет присутствует ли заданная ячейка в коллекции.
    /*! \tparam First_ Первая ячейка в коллекции.
        \tparam What_ Идентификатор искомой ячейки.
        Если ячейка с идентификатором `What_` отствует в коллекции 
        или в коллекции нет ни одной ячейки, 
        то данный класс будет унаследован от класса `Utility::False` и
        тип \c Cell описывающий найденную ячейку будет определен как \c Utility::Null, иначе
        класс будет унаследован от `Utility::True` и 
        тип \c Cell будет совпадать с типом найденной ячейки.
    */
    template< class First_, class What_ > struct IsCellExist;

    //! Содержит детали реализации.
    namespace Detail
    {
        //! Реализует поиск ячеек в коллекции.
        /*! \par Алгоритм
            Последовательно рассматриваются все ячейки в коллекции,
            пока не будет найдена нужная или пока не будет достигнут конец коллекции.
            \tparam Current_ Ячейка рассматриваемая в данный момент.
            \tparam What_ Идентификатор искомой ячейки.
            \tparam Bool_ Признак того, является ли текующая ячейка искомой.
        */
        template< class Current_, class What_, class Bool_ > struct IsCellExistImp;
    
        //! Специализация для случая, когда текующая ячейка является искомой.
        template< class Current_, class What_ > struct IsCellExistImp < Current_, What_, Utility::True > :
            public Utility::True
        {
            typedef Current_ Cell; //!< Найденная ячейка.
        };
        
        //! Специализация для случая, когда ячейка еще не найдена.
        template< class Current_, class What_ > struct IsCellExistImp < Current_, What_, Utility::False > :
            public IsCellExist< typename Current_::CellNext, What_ > { };

         //! Реализует обращение последовательности ячеек.
        /*! \tparam CurrentId_ Ячейка, которая должна быть добавлена в начало уже построенной части обращенной последовательности.
            \tparam Tail_ Нерассмотренная часть обращаемой последоватльности ячеек.
            \tparam Reverse_ Уже построенная часть обращенной последовательности.
        */
        template< class CurrentId_, class Tail_, class Reverse_ > struct ReverseCellImp
        {
            //! Обращенная последовательность ячеек.
            typedef typename ReverseCellImp< typename Tail_::CellId,
                                             typename Tail_::CellNext,
                                             Cell< CurrentId_, Reverse_ > >::Result Result;
        };

        //! Реализует обращение последовательности ячеек.
        /*! Специализация для случая, когда обработана вся последовательность ячеек.
        */
        template< class CurrentId_, class Reverse_ > struct ReverseCellImp< CurrentId_, Utility::Null, Reverse_ >
        {
            //! Обращенная последовательность ячеек.
            typedef Cell< CurrentId_, Reverse_ > Result;
        };

        //! Реализует объединение двух последовательностей ячеек.
        template< class CurrentId_, class Tail_, class Merged_ > struct MergeCellImp
        {
            //! Резлультат объединения.
            typedef typename MergeCellImp< typename Tail_::CellId,
                                           typename Tail_::CellNext,
                                           Cell< CurrentId_, Merged_ > >::Result Result;
        };

        //! Реализует объединение двух последовательностей ячеек.
        /*! Специализация для случая, когда левая последовательность ячеек обработана.
        */
        template< class CurrentId_, class Merged_ > struct MergeCellImp< CurrentId_, Utility::Null, Merged_ >
        {
            //! Резлультат объединения.
            typedef Cell < CurrentId_, Merged_ > Result;
        };
    }

    //! Реализация класса, определяющего присутствует ли заданная ячейка в коллекции.
    template< class First_, class What_ > struct IsCellExist : 
        public Detail::IsCellExistImp< First_, What_,
                                       typename Utility::IsIdentical< typename First_::CellId, What_ >::Definition > { };

    //! Специализация для случая, когда достигнут конец коллекции.
    template< class What_ > struct IsCellExist< Utility::Null, What_ > :
        public Utility::False
    {
        typedef Utility::Null Cell; //!< Ячейка не была найдена.
    };

    //! Строит обращение последовательности ячеек.
    /*! \tparam First_ Первая ячейка в последовательности.
    */
    template< class First_ > struct ReverseCell
    {
        //! Обращенная последовательность ячеек.
        typedef typename Detail::ReverseCellImp< typename First_::CellId, 
                                                 typename First_::CellNext, 
                                                 Utility::Null >::Result Result;
    };

    //! Строит обращение последовательности ячеек.
    template< > struct ReverseCell< Utility::Null >
    {
        //! Обращенная последовательность ячеек.
        typedef Utility::Null Result;
    };

    //! Объединяет две последовательности ячеек.
    template< class Left_, class Right_ > struct MergeCell
    {
        //! Резлультат объединения.
        typedef typename Detail::MergeCellImp< typename ReverseCell< Left_ >::Result::CellId,
                                               typename ReverseCell< Left_ >::Result::CellNext,
                                               Right_ >::Result Result;
    };

    //! Объединяет две последовательности ячеек.
    template< class Left_> struct MergeCell< Left_, Utility::Null >
    {
        //! Резлультат объединения.
        typedef Left_ Result;
    };

    //! Объединяет две последовательности ячеек.
    template< class Right_ > struct MergeCell< Utility::Null, Right_ >
    {
        //! Резлультат объединения.
        typedef Right_ Result;
    };

    //! Высвобождает память, если ячека содержит указатель.
    struct ClearCellFunctor
    {
        //! Удаляет данные из заданной ячейки.
        template< class Cell_ > inline void operator( )( Cell_ *cell)
        {
            typedef typename Cell_::CellId CellId;
            typedef typename CellId::ValueType ValueType;

            Clear( cell, Utility::IsPointer< ValueType >( ));
        }
    private:
        //! Высвобождает память выделенную динамически под хранение значения в ячейке.
        template< class Cell_ >
        inline void Clear( Cell_ *cell, Utility::True)
        {
            typedef typename Cell_::CellId CellId;

            if ( cell->template Value< CellId >( ))
                delete cell->template Value< CellId >( );
        }

        //! Метод-заглушка для случая, когда память динамически не выделялась.
        template< class Cell_ > 
        inline void Clear( Cell_ *cell, Utility::False) { }
    };

    //! Инициализирует ячейку-указатель неопределенным значение.
    struct InitCellFunctor
    {
        //! Инициализирует ячейку-указатель неопределенным значение.
        template< class Cell_ > inline void operator( )( Cell_ *cell)
        {
            typedef typename Cell_::CellId CellId;
            typedef typename CellId::ValueType ValueType;

            Init( cell, Utility::IsPointer< ValueType >( ));
        }
    private:
        //! Инициализирует ячейку-указатель неопределенным значение.
        template< class Cell_ >
        inline void Init( Cell_ *cell, Utility::True)
        {
            typedef typename Cell_::CellId CellId;

            cell->template Value< CellId >( ) = NULL;
        }

        //! Метод-заглушка для случая, когда ячейка не содержит указатель.
        template< class Cell_ > 
        inline void Init( Cell_ *cell, Utility::False) { }
	};

    //! Выполняет копирование данных из одной ячейки в другую.
    /*! \tparam From_ Источник копирования.
        \post Значения ячеек в строке назначения, которые присутствуют в источнике копирования,
        получают значения из источника копирования.
    */
    template< class From_ >
    struct CopyToCellFunctor
    {
        typedef From_ From; //!< Источник копирования.

        //! Создает функционал.
        CopyToCellFunctor( const From *from) : m_from( from) { ASSERT( m_from); }

        //! Выполняет копирование данных в заданную ячейку.
        template< class Cell_ > inline void operator( )( Cell_ *cell)
        {
            typedef typename Cell_::CellId CellId;

            CopyTo( cell, IsCellExist< From, CellId >( ));
        }

    private:
        //! Выполняет копирование данных в заданную ячейку.
        /*! Перегрузка для случая, когда копируемая ячейка присутствует в источнике копирования.
        */
        template< class Cell_ > inline void CopyTo( Cell_ *cell, Utility::True)
        {
            typedef typename Cell_::CellId CellId;
            cell->template Value< CellId >( ) = m_from->template Value< CellId >( );
        }

        //! Выполняет копирование данных в заданную ячейку.
        /*! Перегрузка для случая, когда копируемая ячейка отсутствует в источнике копирования.
        */
        template< class Cell_ > inline void CopyTo( Cell_ *, Utility::False) { }

    private:
        const From *m_from; //!< Значение источника.
    };

    //! Выполняет копирование данных из одной ячейки в другую.
    /*! \tparam From_ Назначение копирования.
        \post Значения ячеек в строке назначения, которые присутствуют в источнике копирования,
        получают значения из источника копирования.
    */
    template< class To_ >
    struct CopyFromCellFunctor
    {
        typedef To_ To; //!< Назначение копирования.

        //! Создает функционал.
        CopyFromCellFunctor( const To *to) : m_to( to) { ASSERT( m_to); }

        //! Выполняет копирование данных из заданной ячейки.
        template< class Cell_ > inline void operator( )( Cell_ *cell)
        {
            typedef typename Cell_::CellId CellId;
            CopyFrom( cell, IsCellExist< To, CellId >( ));
        }

    private:
        //! Выполняет копирование данных из заданной ячейки.
        /*! Перегрузка для случая, когда копируемая ячейка пристутсвует в строке назначения.
        */
        template< class Cell_ > inline void CopyFrom( Cell_ *cell, Utility::True)
        {
            typedef typename Cell_::CellId CellId;
            m_to->template Value< CellId >( ) = cell->template Value< CellId >( );
        }

        //! Выполняет копирование данных из заданной ячейки.
        /*! Перегрузка для случая, когда копируемая ячейка отстутсвует в строке назначения.
        */
        template< class Cell_ > inline void CopyFrom( Cell_ *, Utility::False) { }

    private:
        const To *m_to; //!< Назначение копирования.
    };
}

/*! \page cell_test	Пример использования класса Base::Cell. Данные о зарплате 
	Необходимо создать реестр с записями вида:
	Имя | Зарплата
	--- | --------
	Смит| 300.50
	Для каждой записи должна быть предусмотрена возможность печати.
	О человеке будем хрантить информацию в виде коллекции ячеек \c Base::Cell с идентификаторами \c Name и \c Salary.
	Исходный код примера доступен из файла \a \b cell_test.cpp:	
	
	\dontinclude cell_test.cpp
	1. Подключаем необходимые файлы с определениями:
	\until iostream
	2. Разрешаем доступ к пространству имен \c Base:
	\skipline using
	3. Описываем идентификаторы ячеек, задающие структуру хранимой информации:
	\n Имя человека:		
	\skip Name
	\until ;
	Имя всегда должно быть задано:
	\skipline Nullable
	Создаем статический метод печатающий название ячейки:
	\until };
	Зарплата человека:		
	\skip Salary		
	\until ;
	Если человек нигде не работает, то его зарплата не определена:
	\until };
	4. \anchor cell_def Описываем структуру записи, сосотящей из двух ячеек:
	\skip CELL
	\line CELL
	\line CELL
	5. \anchor cell_foreach_definition
	Создаем функционал для печати информации о стркутуре записи 
	с помощью статического метода \c Base::Cell::ForeachDefinition( ):
	\skip Functor
	\until };
	6. \anchor cell_foreach
	Создаем функционал для печати информации о человеке \c Base::Cell::Foreach( ):
	\skip Functor
	\until };
	7. Создаем пустую запись, заполняем ее значениями и печатаем.
	Имя должно быть задано до попытки его чтения (определяется спецификацией ячейки), 
	иначе при печати результат будет неопределен:
	\skip main
	\until }
	В результате запуска программы получим:		
	\n `Structure of salary information collection:`
	\n `Name is <value>`
	\n `Salary is <value>`
	\n `Worker's salary before he starts wroking:`
	\n `Name is Smit`
	\n `Salary is NULL`
	\n `Worker's salary after he starts wroking:`
	\n `Name is Smit`
	\n `Salary is 300.5`
*/

#endif//СELL_H
