/* 
 * File:   DefuzzifierFactory.h
 * Author: jcrada
 *
 * Created on 8 January 2013, 11:17 PM
 */

#ifndef FL_DEFUZZIFIERFACTORY_H
#define	FL_DEFUZZIFIERFACTORY_H

#include "fl/definitions.h"

#include <string>

namespace fl {
    class Defuzzifier;
    
    class DefuzzifierFactory {
    public:
        DefuzzifierFactory();
        virtual ~DefuzzifierFactory();

        virtual Defuzzifier* create(const std::string& className,
                int divisions = FL_DEFAULT_DIVISIONS);
    };
}
#endif	/* DEFUZZIFIERFACTORY_H */
