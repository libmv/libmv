// Copyright (c) 2010 libmv authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.


#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/descriptor_factory.h"
#include "libmv/descriptor/simpliest_descriptor.h"
#include "libmv/descriptor/dipole_descriptor.h"
#include "libmv/descriptor/surf_descriptor.h"
#include "libmv/descriptor/daisy_descriptor.h"
#include "libmv/logging/logging.h"

namespace libmv {
namespace descriptor {

Describer *describerFactory(eDescriber edescriber)  {

  switch (edescriber)
  {
  case SIMPLEST_DESCRIBER:
    return descriptor::CreateSimpliestDescriber();
    break;
  case DIPOLE_DESCRIBER:
    return descriptor::CreateDipoleDescriber();
    break;
  case SURF_DESCRIBER:
    return descriptor::CreateSurfDescriber();
    break;
  case DAISY_DESCRIBER:
    return descriptor::CreateDaisyDescriber();
    break;
  default:
    LOG(FATAL) << "ERROR : undefined Describer value : " << edescriber;
  }
  return NULL;
}

}  // namespace detector
}  // namespace libmv
