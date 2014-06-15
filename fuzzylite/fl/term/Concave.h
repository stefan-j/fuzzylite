// #BEGIN_LICENSE
// fuzzylite: a fuzzy logic control library in C++
// Copyright (C) 2014  Juan Rada-Vilela
// 
// This file is part of fuzzylite.
//
// fuzzylite is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// fuzzylite is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fuzzylite.  If not, see <http://www.gnu.org/licenses/>.
// #END_LICENSE

/* 
 * File:   Concave.h
 * Author: jcrada
 *
 * Created on 4 June 2014, 12:12 PM
 */

#ifndef FL_CONCAVE_H
#define	FL_CONCAVE_H

#include "fl/term/Term.h"


namespace fl {

    class FL_EXPORT Concave : public Term {
    protected:
        scalar _inflection, _end;
    public:
        Concave(const std::string& name = "",
                scalar inflection = fl::nan,
                scalar end = fl::nan, 
                scalar height = 1.0);
        virtual ~Concave();

        virtual std::string className() const;
        virtual std::string parameters() const;
        virtual void configure(const std::string& parameters);

        virtual scalar membership(scalar x) const;

        virtual void setInflection(scalar inflection);
        virtual scalar getInflection() const;

        virtual void setEnd(scalar end);
        virtual scalar getEnd() const;

        virtual Concave* clone() const;

        static Term* constructor();
    };
}


#endif	/* FL_CONCAVE_H */

