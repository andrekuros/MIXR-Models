
#ifndef __asa_components_factory_H__
#define __asa_components_factory_H__

#include "mixr/base/Object.hpp"
#include <string>

namespace asa {
    namespace components {

        mixr::base::Object* factory(const std::string& name);

    }
}

#endif
