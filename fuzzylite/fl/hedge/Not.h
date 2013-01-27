/*
 * Not.h
 *
 *  Created on: 4/12/2012
 *      Author: jcrada
 */

#ifndef FL_NOT_H
#define FL_NOT_H

#include "fl/hedge/Hedge.h"

namespace fl {

    class FL_EXPORT Not : public Hedge {
    public:
        std::string name() const;
        scalar hedge(scalar x) const;
    };

} 
#endif /* FL_NOT_H */
