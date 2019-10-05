/*! \file
    \brief Содержит классы для создания и использования статического дерева.
*/
#ifndef STATIC_TREE_H
#define STATIC_TREE_H

#include "utility.h"

namespace Base
{
    //! Описание найденной вершины.
    /*! \tparam First_ Найденная вершина.
    */
    template< class First_ > struct RequiredNode
        : public Utility::True
    {
        typedef First_ Result;
    };

    //! Определяет присутствует ли заданная вершина в дереве.
    /*! \tparam First_ Корень дерева.
        \tparam What_ Идентификатор искомой веширны.
        Если вершина с идентификатором `What_` отствует в дереве
        или в дереве нет ни одной вершины, 
        то данный класс будет унаследован от класса `Utility::False` и
        тип \c Result описывающий найденную ячйеку будет определен как \c Utility::Null, иначе
        класс будет унаследован от `Utility::True` и 
        тип \c Result будет совпадать с типом найденной ячейки.
    */
    template< class First_, class What_ > struct IsNodeExist
        : public Utility::If< Utility::IsIdentical< typename First_::Id, What_ >,
                              RequiredNode< First_ >, 
                              typename Utility::If< IsNodeExist< typename First_::Left, What_>,
                                                    IsNodeExist< typename First_::Left, What_>,
                                                    IsNodeExist< typename First_::Right, What_> >::Result >::Result { };

    //! Перегрузка для случая когда не удалось найти вершину.
    template< class What_ > struct IsNodeExist< Utility::Null, What_ >
        : public Utility::False
    {
        typedef Utility::Null Result; //!< Вершина не была найдена.
    };

    //! Дуга от вершины к левому поддереву.
    /*! \tparam Left_ Корень левого поддерева.
    */
    template< class Left_ > class LeftBranchImp : public Left_
    { 
    protected:
        //! Создает дугу.
        LeftBranchImp< Left_ >( ) { }

        //! Создает дугу.
        LeftBranchImp< Left_ >( const Left_ &left) : Left_( left) { }
    };

    //! Дуга от вершины к правому поддереву.
    /*! \tparam Right_ Корень правого поддерева.
    */
    template< class Right_ > class RightBranchImp : public Right_ 
    { 
    protected:
        //! Создает дугу.
        RightBranchImp< Right_ >( ) { }

        //! Создает дугу.
        RightBranchImp< Right_ >( const Right_ &right) : Right_( right) { }
    };

    //! Статическое дерево.
    /*! \tparam Id_ Идентификатор кореня дерева.
        \tparam Left_ Левое поддерево.
        \tparam Right_ Правое поддерево.
    */
    template< class Id_, class Left_ = Utility::Null, class Right_ = Utility::Null >
    class Node : 
        public LeftBranchImp< Left_ >, 
        public RightBranchImp< Right_ >, 
        public virtual Utility::If< Utility::IsReference< typename Id_::ValueType >,
                                    Utility::Unassignable,
                                    Utility::Copyable >::Result
    {
    public:
        typedef Id_ Id; //!< Идентификатор корня дерева.
        typedef typename Id::ValueType ValueType; //!< Тип значений, хранимых в корне.

        typedef Left_ Left; //!< Левое поддерево.
        typedef LeftBranchImp< Left > LeftBranch; //!< Дуга к левому поддереву.

        typedef Right_ Right; //!< Правое поддерево.
        typedef RightBranchImp< Right > RightBranch; //!< Дуга к правому поддереву.

        typedef Node< Id, Left, Right > Root; //!< Корень дерева.

    public:
        //! Создает дерево.
        Node( ) { }

        //! Создает дерево с заданным значением в корне.
        Node( typename Utility::If< Utility::IsReference< ValueType >, ValueType, const ValueType & >::Result value, const Left &left, const Right &right)
            : m_value( value), LeftBranch( left), RightBranch( right) { }

        //! Копирует дерево с заданным корнем.
        template< class Root_ >
        Node( const Root_ &root) : 
            m_value( root.template Value< typename Root_::Id >( )), 
            LeftBranch( root.LeftNode( )), 
            RightBranch( root.RightNode( )) 
        { }

        //! Возвращает левое поддерево.
        Left & LeftNode( ) { return static_cast< Left & >( static_cast< LeftBranch & >( *this)); }

        //! Возвращает левое поддерево.
        const Left & LeftNode( ) const { return static_cast< const Left & >( static_cast< const LeftBranch & >( *this)); }

        //! Возвращает правое поддерево.
        Right & RightNode( ) { return static_cast< Right & >( static_cast< RightBranch & >( *this)); }

        //! Возвращает правое поддерево.
        const Right & RightNode( ) const { return static_cast< const Right & >( static_cast< const RightBranch & >( *this)); }

        //! \name Access-методы
        //@{
        //! Предоставляет доступ к значению, хранимому в вершине.
        /*! \tparam What_ Идентификатор вершины.
        */
        template< class What_ > 
        typename What_::ValueType & Value( ) 
        {
            return IsNodeExist< Root, What_ >::Result::m_value;
        }

        //! Возвращает значение, хранимое в вершине.
        /*! \tparam What_ Идентификатор вершины.
        */
        template< class What_ > 
        const typename What_::ValueType & Value( ) const 
        {
            return IsNodeExist< Root, What_ >::Result::m_value;
        }

        //! Предоставляет доступ к значению, хранимому в вершине.
        /*! \tparam What_ Идентификатор вершины
        */
        template< class What_ > 
        typename What_::ValueType & operator[ ]( What_) { return Value< What_ >( ); }

        //! Возвращает значение, хранимое в вершине.
        /*! \tparam What_ Идентификатор вершины.
        */
        template< class What_ > 
        const typename What_::ValueType & operator[ ]( What_) const { return Value< What_ >( ); }
        //@}

    protected:
        ValueType m_value; //!< Значение хранимое в ячейке.
    };

    //! Выполняет функционал над всеми вершинами дерева.
    /*! \tparam Root_ Корень дерева.
        \tparam Functor_ Функционал который должен быть выполнен.
        \param [in, out] root Корень дерева.
        \param [in, out] functor Функционал.
    */
    template< class Root_, class Functor_ >
    void Visit( Root_ &root, Functor_ &functor)
    {
        Visit( root.LeftNode( ), functor);
        Visit( root.RightNode( ), functor);
        functor( root);
    }
}

#endif//STATIC_TREE_H
