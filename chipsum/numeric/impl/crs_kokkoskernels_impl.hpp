/* * * * * * * * * * * * * * * * * * * * *
*   File:     crs_kokkoskernels_impl.hpp
*   Author:   Li Kunyun
*   group:    CDCS-HPC
*   Time:     2021-07-28
* * * * * * * * * * * * * * * * * * * * * */

#ifndef __CHIPSUM_CRS_KOKKOSKERNELS_IMPL_HPP__
#define __CHIPSUM_CRS_KOKKOSKERNELS_IMPL_HPP__


#include <KokkosKernels_default_types.hpp>
#include <KokkosSparse.hpp>
#include <KokkosSparse_spmv.hpp>
#include <KokkosSparse_trsv.hpp>
#include <KokkosSparse_sptrsv.hpp>

#include "../numeric_traits.hpp"
#include "../sparse_matrix_types.h"
#include "../../chipsum_macro.h"


static int spm_name = 0;

namespace ChipSum{
namespace Numeric {

template<typename ScalarType,typename SizeType,typename ...Props>
struct Sparse_Traits<ScalarType,SizeType,Csr,ChipSum::Backend::KokkosKernels,Props...>
        : public Operator_Traits<ScalarType,SizeType,ChipSum::Backend::KokkosKernels,Props...>{


    using matrix_format_type = KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>;

    using graph_type = typename matrix_format_type::staticcrsgraph_type;
    using row_map_type = typename matrix_format_type::row_map_type;
    using col_map_type = typename matrix_format_type::index_type;
    using matrix_values_type = typename matrix_format_type::values_type;


};


namespace Impl {

namespace Sparse {



template<typename ScalarType,typename SizeType,typename ...Props>
/**
 * @brief Fill：利用POD数据对稀疏矩阵进行填充
 * @param A：稀疏矩阵
 * @param nrows：行数
 * @param ncols：列数
 * @param annz：非零元数
 * @param row_map：行向量
 * @param col_map：entry向量
 * @param values：非零元向量
 */
CHIPSUM_FUNCTION_INLINE void Create(
        KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>& A,
        const SizeType nrows,
        const SizeType ncols,
        const SizeType annz,
        SizeType* row_map,
        SizeType* col_map,
        ScalarType* values

        )
{

    using crs_t = KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>;

    A.ctor_impl("spm_"+std::to_string(spm_name),
                static_cast<typename crs_t::ordinal_type>(nrows),
                static_cast<typename crs_t::ordinal_type>(ncols),
                static_cast<typename crs_t::size_type>(annz),
                static_cast<typename crs_t::value_type*>(values),
                static_cast<typename crs_t::ordinal_type*>(row_map),
                static_cast<typename crs_t::ordinal_type*>(col_map));

}


template<typename ScalarType,typename SizeType,typename ...Props>
/**
 * @brief Spmv
 * @param A
 * @param x
 * @param b
 */
CHIPSUM_FUNCTION_INLINE void Mult(
        KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>& A,
        Kokkos::View<ScalarType*>& x,
        Kokkos::View<ScalarType*>& b)
{
    KokkosSparse::spmv("N",static_cast<ScalarType>(1.0),A,x,static_cast<ScalarType>(0.0),b);
}


template<typename ScalarType,typename SizeType,typename ...Props>
/**
 * @brief Spmv
 * @param alpha
 * @param A
 * @param x
 * @param beta
 * @param b
 */
CHIPSUM_FUNCTION_INLINE void Mult(
        ScalarType alpha,
        KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>& A,
        Kokkos::View<ScalarType*>& x,
        ScalarType beta,
        Kokkos::View<ScalarType*> b)
{
    KokkosSparse::spmv("N",alpha,A,x,beta,b);
}


template <typename ScalarType,typename SizeType,typename ...Props>
CHIPSUM_FUNCTION_INLINE void Create(
        KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>& A,
        const size_t row_map_size,
        const size_t col_map_size
        )
{
    using sp_t = KokkosSparse::CrsMatrix<ScalarType,SizeType,default_device>;


    typename sp_t::row_map_type row_map(
                "row_map_"+A.label(),
                static_cast<typename sp_t::row_map_type>(row_map_size)
                );
    typename sp_t::entries_type col_map(
                "col_map_"+A.label(),
                static_cast<typename sp_t::entries_type>(col_map_size)
                );

    typename sp_t::staticcrsgraph_type graph(col_map,row_map);

    A = spt(A.label(),graph);


    //    using row_map_type =
    //        View<const size_type*, array_layout, device_type, memory_traits>;
    //    using entries_type =
    //        View<data_type*, array_layout, device_type, memory_traits>;
    //    using row_block_type =
    //        View<const size_type*, array_layout, device_type, memory_traits>;


}

} // End namespace Sparse
} // End namespace Impl
} // End namespace Numeric
} // End namespace ChipSum

#endif // __CHIPSUM_CRS_KOKKOSKERNELS_IMPL_HPP__
