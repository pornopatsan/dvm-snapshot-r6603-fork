/*! \file
    \brief Содержит конструкции для работы с текстовыми строками.
*/

#ifndef TEXT_H
#define TEXT_H

#include "declaration.h"
#include "exception.h"

#include <stdlib.h>
#include <string.h>

namespace Base
{
     //! Копирует строку в формате UNICODE
    inline void CopyUnicode( CharUnicode *to, size_t toSize, const CharUnicode *from)
    {
#ifdef __GNUC__
        if ( wcslen( from) < toSize)
            wcscpy( to, from);
        else
#else
        if ( wcscpy_s( to, toSize, from) != 0)
#endif
            throw Base::Exception< BCL >::Error< CELL_COLL_1( ErrorList::CopyString) >( );
    }

    //! Преобразует число в строку в формате UNICODE.
    inline TextUnicode ToTextUnicode( int value)
    {
        CharUnicode buffer[ 20];

        int last = 0;
        int index = 0;

        if ( value == 0)
            return L"0";

        if ( value < 0)
        {
            buffer[ index++] = '-';
            value = -value;
        }

        for ( int tmp = value; tmp > 0; tmp /= 10, last++);

        if ( index + last > 19)
            throw  Base::Exception< BCL >::Error< CELL_COLL_1( ErrorList::ConvertString) >( );

        buffer[ index + last] = '\0';
        for ( index = index + last - 1; value > 0; value /= 10, index--)
            buffer[ index] = value % 10 + '0';

        return buffer;
    }

    //! Копирует строку в формате ANSI
    inline void CopyAnsi( CharAnsi *to, size_t toSize, const CharAnsi *from)
    {
#ifdef __GNUC__
        if ( strlen( from) < toSize)
            strcpy( to, from);
        else
#else
        if ( strcpy_s( to, toSize, from) != 0)
#endif
            throw Base::Exception< BCL >::Error< CELL_COLL_1( ErrorList::CopyString) >( );
    }

    //! Преобразует число в строку в формате ANSI.
    inline TextAnsi ToTextAnsi( int value)
    {
        CharAnsi buffer[ 20];
        int last = 0;
        int index = 0;

        if ( value == 0)
            return "0";

        if ( value < 0)
        {
            buffer[ index++] = '-';
            value = -value;
        }

        for ( int tmp = value; tmp > 0; tmp /= 10, last++);

        if ( index + last > 19)
            throw  Base::Exception< BCL >::Error< CELL_COLL_1( ErrorList::ConvertString) >( );

        buffer[ index + last] = '\0';
        for ( index = index + last - 1; value > 0; value /= 10, index--)
            buffer[ index] = value % 10 + '0';

        return buffer; 
    }

#ifdef UNICODE
    //! Преобразует число в строку в формате UNICODE.
    inline Text ToText( int value) { return ToTextUnicode( value); }
    //! Копирует строки в формате UNICODE.
    inline void CopyString( Char *to, size_t toSize, const Char *from)
    {
        CopyUnicode( to, toSize, from); 
    }
#else
    //! Преобразует число в строку в формате ANSI.
    inline Text ToText( int value) { return ToTextAnsi( value); }
    //! Копирует строки в формате ANSI.
    inline void CopyString( Char *to, size_t toSize, const Char *from)
    {
        CopyAnsi( to, toSize, from); 
    }
#endif
}

#endif//TEXT_H
