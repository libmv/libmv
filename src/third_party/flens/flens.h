/*
 *   Copyright (c) 2007, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLENS_FLENS_H
#define FLENS_FLENS_H 1

#define ADDRESS(x) reinterpret_cast<const void *>(&x)

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif //ASSERT

#include <flens/array.h>
#include <flens/bandstorage.h>
#include <flens/blas.h>
#include <flens/blas_flens.h>
#include <flens/cg.h>
#include <flens/complex_helper.h>
#include <flens/crs.h>
#include <flens/densevector.h>
#include <flens/evalclosure.h>
#include <flens/fixedsizearray.h>
#include <flens/fullstorage.h>
#include <flens/generalmatrix.h>
#include <flens/refcounter.h>
#include <flens/lapack.h>
#include <flens/lapack_flens.h>
#include <flens/lapack_flens_aux.h>
#include <flens/listinitializer.h>
#include <flens/matvec.h>
#include <flens/matvecclosures.h>
#include <flens/matvecio.h>
#include <flens/matvecoperations.h>
#include <flens/multigrid.h>
#include <flens/packedstorage.h>
#include <flens/polynom.h>
#include <flens/range.h>
#include <flens/snapshot.h>
#include <flens/storage.h>
#include <flens/sparsematrix.h>
#include <flens/sparse_blas.h>
#include <flens/sparse_blas_flens.h>
#include <flens/symmetricmatrix.h>
#include <flens/tiny_blas.h>
#include <flens/tinymatrix.h>
#include <flens/tinyvector.h>
#include <flens/traits.h>
#include <flens/triangularmatrix.h>
#include <flens/underscore.h>
#include <flens/uplo.h>

#endif // FLENS_FLENS_H
